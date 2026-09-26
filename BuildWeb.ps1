. "$PSScriptRoot/tools/common.ps1"
Push-Location $FroggerRoot
try {
    if (-not (Test-Path godot/rom/maincpu.bin)) {
        python tools/prepare_rom.py
        Assert-FroggerExit 'ROM verification'
    }

    $wasmBinary = "godot/bin/libfrogger_arcade.web.template_release.wasm32.wasm"
    $cppSources = Get-ChildItem -Path src -Filter *.cpp -Recurse
    $needCompile = (-not (Test-Path $wasmBinary))
    if (-not $needCompile) {
        $wasmTime = (Get-Item $wasmBinary).LastWriteTime
        foreach ($src in $cppSources) {
            if ($src.LastWriteTime -gt $wasmTime) {
                $needCompile = $true
                break
            }
        }
    }

    if ($needCompile) {
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

        $godotCppLib = "src/godot-cpp/bin/libgodot-cpp.web.template_release.wasm32.a"
        # The stamp records that this lib was built WITH threads=yes. A lib left
        # over from before that flag existed is silently single-threaded and scons
        # would never rebuild it, so the stamp forces exactly one rebuild on upgrade.
        $godotCppThreadsStamp = "src/godot-cpp/bin/.frogger-web-threads-yes"
        if ((-not (Test-Path $godotCppLib)) -or (-not (Test-Path $godotCppThreadsStamp))) {
            Write-Host "Building godot-cpp web library (threads=yes)..."
            Push-Location "src/godot-cpp"
            try {
                scons platform=web target=template_release api_version=4.7 threads=yes
                Assert-FroggerExit 'Build godot-cpp web library'
            } finally { Pop-Location }
            New-Item -ItemType Directory -Force (Split-Path -Parent $godotCppThreadsStamp) | Out-Null
            New-Item -ItemType File -Force $godotCppThreadsStamp | Out-Null
        }

        New-Item -ItemType Directory -Force godot/bin | Out-Null
        $cmd = @(
            "-pthread",
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
        Copy-Item -LiteralPath $wasmBinary -Destination "godot/bin/libfrogger_arcade.web.template_debug.wasm32.wasm" -Force
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
            [System.IO.File]::WriteAllBytes("$sideWasm.part0", $part0)
            [System.IO.File]::WriteAllBytes("$sideWasm.part1", $part1)
            Remove-Item -LiteralPath $sideWasm -Force
            Write-Host "Created index.side.wasm.part0 and index.side.wasm.part1 (< 25 MiB each)"
        }
    }

    # Inject side wasm chunk loader into index.html
    $indexHtmlPath = Join-Path $FroggerRoot "builds/web/index.html"
    if (Test-Path -LiteralPath $indexHtmlPath) {
        $html = [System.IO.File]::ReadAllText($indexHtmlPath)
        # Register the first-gesture audio handler before loading the engine.
        $html = $html.Replace('<script src="index.js"></script>', '<script src="frogger-audio.js"></script><script src="index.js"></script>')
        $chunkScript = @"
		<script>
(function() {
	const origFetch = window.fetch;
	window.fetch = async function(resource, init) {
		const url = (typeof resource === 'string') ? resource : (resource && resource.url) ? resource.url : '';
		if (url.endsWith('.side.wasm')) {
			const [r0, r1] = await Promise.all([
				origFetch(url + '.part0', init),
				origFetch(url + '.part1', init)
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
					'Content-Type': 'application/wasm',
					'Cross-Origin-Opener-Policy': 'same-origin',
					'Cross-Origin-Embedder-Policy': 'require-corp'
				}
			});
		}
		return origFetch(resource, init);
	};
})();
		</script>
"@
        if ($html -notmatch 'side\.wasm\.part0') {
            $html = $html.Replace('<script src="index.js"></script>', "$chunkScript`r`n		<script src=`"index.js`"></script>")
        }
        if ($html -notmatch 'manifest\.webmanifest') {
            $pwaTags = @"
		<link rel="manifest" href="manifest.webmanifest">
		<meta name="mobile-web-app-capable" content="yes">
		<meta name="apple-mobile-web-app-capable" content="yes">
		<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
		<meta name="apple-mobile-web-app-title" content="Frogger">
		<meta name="theme-color" content="#182332">
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
  "theme_color": "#182332",
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
	// Let browser handle fetches natively with Cloudflare edge headers
});
"@
    Set-Content -LiteralPath $swPath -Value $swContent -Encoding ascii

    # Generate PWA 192 and 512 icons if missing
    $icon192 = Join-Path $FroggerRoot "builds/web/icon-192.png"
    $icon512 = Join-Path $FroggerRoot "builds/web/icon-512.png"
    $appleIcon = Join-Path $FroggerRoot "builds/web/index.apple-touch-icon.png"
    if (Test-Path -LiteralPath $appleIcon) {
        if (-not (Test-Path -LiteralPath $icon192) -or -not (Test-Path -LiteralPath $icon512)) {
            python -c "
from PIL import Image
im = Image.open(r'$appleIcon')
im.resize((192, 192), Image.Resampling.LANCZOS).save(r'$icon192')
im.resize((512, 512), Image.Resampling.LANCZOS).save(r'$icon512')
" | Out-Null
        }
    }

    $headersPath = Join-Path $FroggerRoot "builds/web/_headers"
    $headersContent = @"
/*
  Cross-Origin-Opener-Policy: same-origin
  Cross-Origin-Embedder-Policy: require-corp
  Cross-Origin-Resource-Policy: same-origin
"@
    Set-Content -LiteralPath $headersPath -Value $headersContent -Encoding ascii

    Write-Host "Exported $froggerTarget"
    Write-Host "Generated $headersPath"
} finally { Pop-Location }
