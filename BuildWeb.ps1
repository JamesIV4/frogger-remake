. "$PSScriptRoot/tools/common.ps1"
Push-Location $FroggerRoot
try {
    if (-not (Test-Path godot/rom/maincpu.bin)) {
        python tools/prepare_rom.py
        Assert-FroggerExit 'ROM verification'
    }

    $wasmBinary = "godot/bin/libfrogger_arcade.web.template_release.wasm32.wasm"
    $wasmDebugBinary = "godot/bin/libfrogger_arcade.web.template_debug.wasm32.wasm"
    $godotCppLib = "src/godot-cpp/bin/libgodot-cpp.web.template_release.wasm32.nothreads.a"
    # Force migration of existing threaded binaries even if sources are older.
    # Write this stamp only after both non-threaded outputs are complete.
    $wasmBuildStamp = "godot/bin/.frogger-web-nothreads"
    $cppSources = Get-ChildItem -Path src -Recurse -File | Where-Object { $_.Extension -in '.cpp', '.h', '.hpp' }
    $compileInputs = @($cppSources) + @(Get-Item -LiteralPath $PSCommandPath)
    if (Test-Path -LiteralPath $godotCppLib) { $compileInputs += Get-Item -LiteralPath $godotCppLib }
    $needCompile = (-not (Test-Path $wasmBinary)) -or (-not (Test-Path $wasmDebugBinary)) -or (-not (Test-Path $wasmBuildStamp)) -or (-not (Test-Path $godotCppLib))
    if (-not $needCompile) {
        $wasmTime = (Get-Item $wasmBinary).LastWriteTime
        foreach ($src in $compileInputs) {
            if ($src.LastWriteTime -gt $wasmTime) {
                $needCompile = $true
                break
            }
        }
    }

    if ($needCompile) {
        if (Test-Path -LiteralPath $wasmBuildStamp) { Remove-Item -LiteralPath $wasmBuildStamp -Force }
        Write-Host "Compiling GDExtension WebAssembly module..."
        if (-not (Get-Command em++ -ErrorAction SilentlyContinue)) {
            $emsdkCandidates = @(
                "$env:EMSDK\emsdk_env.ps1",
                "$env:USERPROFILE\scoop\apps\emscripten\current\emsdk_env.ps1",
                "C:\emsdk\emsdk_env.ps1"
            )
            foreach ($candidate in $emsdkCandidates) {
                if ($candidate -and (Test-Path -LiteralPath $candidate)) {
                    $env:EMSDK_QUIET = "1"
                    . $candidate | Out-Null
                    break
                }
            }
        }
        if (-not (Get-Command em++ -ErrorAction SilentlyContinue)) {
            throw "em++ not found. Please activate Emscripten SDK."
        }

        Write-Host "Building godot-cpp web library (threads=no)..."
        Push-Location "src/godot-cpp"
        try {
            scons platform=web target=template_release api_version=4.7 threads=no
            Assert-FroggerExit 'Build godot-cpp web library'
        } finally { Pop-Location }

        New-Item -ItemType Directory -Force godot/bin | Out-Null
        $cmd = @(
            "-sSIDE_MODULE=1",
            "-sWASM_BIGINT",
            "-sSUPPORT_LONGJMP=wasm",
            "-O3",
            "-std=c++17",
            "-DWEB_ENABLED",
            "-DUNIX_ENABLED",
            "-Isrc",
            "-Isrc/godot-cpp/include",
            "-Isrc/godot-cpp/gen/include",
            "-Isrc/godot-cpp/gdextension",
            "src/ArcadeSimulation.cpp",
            "src/ArcadeSimulationExtension.cpp",
            "src/NativeSound.cpp",
            "src/Generated/MainProgram.cpp",
            "src/Generated/SoundProgram.cpp",
            "src/register_types.cpp",
            $godotCppLib,
            "-o",
            $wasmBinary
        )
        & em++ @cmd
        Assert-FroggerExit 'Compile GDExtension wasm'
        Copy-Item -LiteralPath $wasmBinary -Destination $wasmDebugBinary -Force
        Set-Content -LiteralPath $wasmBuildStamp -Value 'threads=no' -Encoding ascii
    }

    New-Item -ItemType Directory -Force builds/web | Out-Null
    $froggerExportOut = Join-Path $FroggerRoot 'docs/evidence/export-web-stdout.log'
    $froggerExportErr = Join-Path $FroggerRoot 'docs/evidence/export-web-stderr.log'
    $froggerProject = (Join-Path $FroggerRoot 'godot')
    $froggerTarget = (Join-Path $FroggerRoot 'builds/web/index.html')

    $godotExe = Get-FroggerGodotStandard
    Write-Host "Exporting Web target using $godotExe..."
    $froggerExport = Start-Process -FilePath $godotExe -ArgumentList '--headless','--path',"`"$froggerProject`"",'--export-release','"Web"',"`"$froggerTarget`"" -WindowStyle Hidden -RedirectStandardOutput $froggerExportOut -RedirectStandardError $froggerExportErr -Wait -PassThru
    if ($froggerExport.ExitCode -ne 0 -or (Select-String -LiteralPath $froggerExportErr -Pattern '^ERROR:' -Quiet)) {
        throw "Web export failed. See $froggerExportErr."
    }

    Copy-Item -LiteralPath tools/web_audio.js -Destination builds/web/frogger-audio.js -Force

    New-Item -ItemType Directory -Force builds/web/licenses | Out-Null
    Copy-Item -LiteralPath LICENSE,THIRD_PARTY.md -Destination builds/web/licenses -Force
    Get-ChildItem -LiteralPath godot/Fonts -Filter '*-OFL.txt' | Copy-Item -Destination builds/web/licenses -Force

    # Cloudflare Pages 25 MiB file size limit: split index.side.wasm if > 25MB
    $sideWasm = Join-Path $FroggerRoot "builds/web/index.side.wasm"
    $sideWasmPart0 = Join-Path $FroggerRoot "builds/web/index.side.part0.wasm"
    $sideWasmPart1 = Join-Path $FroggerRoot "builds/web/index.side.part1.wasm"
    # Clear both the current names and the old extension-less names. Ending the
    # deployed chunks in .wasm gives Pages the application/wasm content type,
    # which allows Cloudflare's automatic Brotli/Gzip delivery to engage.
    foreach ($oldPart in @($sideWasmPart0, $sideWasmPart1, "$sideWasm.part0", "$sideWasm.part1")) {
        if (Test-Path -LiteralPath $oldPart) {
            Remove-Item -LiteralPath $oldPart -Force
        }
    }
    if (Test-Path -LiteralPath $sideWasm) {
        $size = (Get-Item $sideWasm).Length
        if ($size -gt 25MB) {
            Write-Host "Splitting $sideWasm ($([math]::Round($size/1MB, 1)) MB) into Cloudflare-compatible chunks..." -ForegroundColor Cyan
            $bytes = [System.IO.File]::ReadAllBytes($sideWasm)
            $half = [int]([math]::Ceiling($bytes.Length / 2.0))
            $part0 = [byte[]]::new($half)
            $part1 = [byte[]]::new($bytes.Length - $half)
            [System.Array]::Copy($bytes, 0, $part0, 0, $half)
            [System.Array]::Copy($bytes, $half, $part1, 0, $bytes.Length - $half)
            [System.IO.File]::WriteAllBytes($sideWasmPart0, $part0)
            [System.IO.File]::WriteAllBytes($sideWasmPart1, $part1)
            Remove-Item -LiteralPath $sideWasm -Force
            Write-Host "Created index.side.part0.wasm and index.side.part1.wasm (< 25 MiB each)"
        }
    }

    # Version the browser's IndexedDB asset cache by commit and by the exact
    # exported bytes. The content suffix protects dirty local deployments from
    # reusing binaries produced from an earlier build of the same commit.
    $commitHash = (& git rev-parse --short=12 HEAD 2>$null | Out-String).Trim()
    if (-not $commitHash) { $commitHash = 'no-git' }
    $cacheAssetPaths = @(
        'builds/web/index.pck',
        'builds/web/index.wasm',
        'builds/web/index.side.wasm',
        'builds/web/index.side.part0.wasm',
        'builds/web/index.side.part1.wasm',
        'builds/web/libfrogger_arcade.web.template_release.wasm32.wasm'
    ) | Where-Object { Test-Path -LiteralPath $_ }
    $assetHashText = ($cacheAssetPaths | Sort-Object | ForEach-Object { (Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash }) -join ''
    $hashAlgorithm = [System.Security.Cryptography.SHA256]::Create()
    try {
        $contentDigest = ([System.BitConverter]::ToString($hashAlgorithm.ComputeHash([System.Text.Encoding]::UTF8.GetBytes($assetHashText))) -replace '-', '').ToLowerInvariant().Substring(0, 12)
    } finally {
        $hashAlgorithm.Dispose()
    }
    $cacheVersion = "$commitHash-$contentDigest"
    $cacheScript = [System.IO.File]::ReadAllText((Join-Path $FroggerRoot 'tools/web_cache.js')).Replace('__FROGGER_CACHE_VERSION__', $cacheVersion)
    [System.IO.File]::WriteAllText((Join-Path $FroggerRoot 'builds/web/frogger-cache.js'), $cacheScript)
    Write-Host "IndexedDB asset cache version: $cacheVersion"

    # Inject side wasm chunk loader into index.html
    $indexHtmlPath = Join-Path $FroggerRoot "builds/web/index.html"
    if (Test-Path -LiteralPath $indexHtmlPath) {
        $html = [System.IO.File]::ReadAllText($indexHtmlPath)
        # Register the first-gesture audio handler before loading the engine.
        $html = $html.Replace('<script src="index.js"></script>', '<script src="frogger-cache.js"></script><script src="frogger-audio.js"></script><script src="index.js"></script>')
        # The browser PCM transport owns the sole audio context. Use Godot's
        # supported Dummy driver rather than leaving a second silent worklet alive.
        $audioStartup = "engine.startGame({"
        if (-not $html.Contains($audioStartup)) { throw 'Web shell audio startup hook missing.' }
        $html = $html.Replace($audioStartup, "$audioStartup`n`t`t`t'args': (GODOT_CONFIG.args || []).concat(window.FroggerAudio.engine_arguments()),")
        $chunkScript = @"
		<script>
(function() {
	const origFetch = window.fetch;
	const cachedFetch = window.FroggerAssetCache ? window.FroggerAssetCache.fetch : origFetch;
	window.fetch = async function(resource, init) {
		const url = (typeof resource === 'string') ? resource : (resource && resource.url) ? resource.url : '';
		if (url.endsWith('.side.wasm')) {
			const [r0, r1] = await Promise.all([
				cachedFetch(url.replace(/\.side\.wasm$/, '.side.part0.wasm'), init),
				cachedFetch(url.replace(/\.side\.wasm$/, '.side.part1.wasm'), init)
			]);
			if (!r0.ok || !r1.ok) {
				throw new Error('Failed to load side wasm parts: ' + r0.status + ' / ' + r1.status);
			}
			const [b0, b1] = await Promise.all([r0.arrayBuffer(), r1.arrayBuffer()]);
			const combined = new Uint8Array(b0.byteLength + b1.byteLength);
			combined.set(new Uint8Array(b0), 0);
			combined.set(new Uint8Array(b1), b0.byteLength);
			return new Response(combined, {
				status: 200,
				statusText: 'OK',
				headers: {
                    'Content-Type': 'application/wasm'
				}
			});
		}
		return cachedFetch(resource, init);
	};
})();
		</script>
"@
        if ((Test-Path -LiteralPath $sideWasmPart0) -and ($html -notmatch 'side\.part0\.wasm')) {
            $html = $html.Replace('<script src="index.js"></script>', "$chunkScript`r`n		<script src=`"index.js`"></script>")
        }
        if ($html -notmatch 'manifest\.webmanifest') {
            $pwaTags = @"
		<link rel="manifest" href="manifest.webmanifest">
		<meta name="mobile-web-app-capable" content="yes">
		<meta name="apple-mobile-web-app-capable" content="yes">
		<meta name="apple-mobile-web-app-status-bar-style" content="black">
		<meta name="apple-mobile-web-app-title" content="Frogger">
		<meta name="theme-color" content="#000000">
		<script>
			if ('serviceWorker' in navigator) {
				window.addEventListener('load', () => {
					navigator.serviceWorker.register('sw.js').catch(() => {});
				});
			}
		</script>
	</head>
"@
            $html = $html.Replace('</head>', $pwaTags)
        }
        [System.IO.File]::WriteAllText($indexHtmlPath, $html)
        Write-Host "Injected side wasm chunk loader and PWA tags into $indexHtmlPath"
    }

    # Generate PWA manifest and service worker
    $manifestPath = Join-Path $FroggerRoot "builds/web/manifest.webmanifest"
    $manifestContent = @"
{
  "name": "Frogger Remake",
  "short_name": "Frogger",
  "description": "1981 Arcade Frogger Remake",
  "start_url": "./index.html",
  "scope": "./",
  "display": "fullscreen",
  "orientation": "portrait",
  "background_color": "#182332",
  "theme_color": "#000000",
  "icons": [
    {
      "src": "icon-192.png",
      "sizes": "192x192",
      "type": "image/png",
      "purpose": "any maskable"
    },
    {
      "src": "icon-512.png",
      "sizes": "512x512",
      "type": "image/png",
      "purpose": "any maskable"
    },
    {
      "src": "index.apple-touch-icon.png",
      "sizes": "180x180",
      "type": "image/png"
    }
  ]
}
"@
    Set-Content -LiteralPath $manifestPath -Value $manifestContent -Encoding ascii

    $swPath = Join-Path $FroggerRoot "builds/web/sw.js"
    $swContent = @"
self.addEventListener('install', (event) => {
	self.skipWaiting();
});

self.addEventListener('activate', (event) => {
	event.waitUntil(self.clients.claim());
});

self.addEventListener('fetch', (event) => {
	// Let the browser handle fetches natively; isolation headers are not needed.
});
"@
    Set-Content -LiteralPath $swPath -Value $swContent -Encoding ascii

    # Refresh PWA icons on every export so SVG changes cannot leave stale icons.
    $icon192 = Join-Path $FroggerRoot "builds/web/icon-192.png"
    $icon512 = Join-Path $FroggerRoot "builds/web/icon-512.png"
    $appleIcon = Join-Path $FroggerRoot "builds/web/index.apple-touch-icon.png"
    if (Test-Path -LiteralPath $appleIcon) {
        python -c "
from PIL import Image
im = Image.open(r'$appleIcon')
im.resize((192, 192), Image.Resampling.LANCZOS).save(r'$icon192')
im.resize((512, 512), Image.Resampling.LANCZOS).save(r'$icon512')
" | Out-Null
        Assert-FroggerExit 'Generate PWA icons'
    }

    $headersPath = Join-Path $FroggerRoot "builds/web/_headers"
    $headersContent = @"
/index.html
  Cache-Control: no-cache
/sw.js
  Cache-Control: no-cache
"@
    Set-Content -LiteralPath $headersPath -Value $headersContent -Encoding ascii

    Write-Host "Exported $froggerTarget"
    Write-Host "Generated $headersPath"
} finally { Pop-Location }
