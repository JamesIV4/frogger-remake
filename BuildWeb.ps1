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

    New-Item -ItemType Directory -Force builds/web/licenses | Out-Null
    Copy-Item -LiteralPath LICENSE,THIRD_PARTY.md -Destination builds/web/licenses -Force
    Get-ChildItem -LiteralPath godot/Fonts -Filter '*-OFL.txt' | Copy-Item -Destination builds/web/licenses -Force

    Write-Host "Exported $froggerTarget"
} finally { Pop-Location }
