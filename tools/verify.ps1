. "$PSScriptRoot/common.ps1"
Push-Location $FroggerRoot
try {
    python tools/prepare_rom.py; Assert-FroggerExit 'ROM verification'
    python tools/validate_assets.py; Assert-FroggerExit 'Asset validation'
    node --test --test-concurrency=2 'tests/*.test.js' 'vendor/arcade-js/games/frogger/test/*.test.js' 'vendor/arcade-js/games/frogger/idiomatic/test/*.test.js' 'vendor/arcade-js/boards/frogger/test/*.test.js'; Assert-FroggerExit 'Recovered-function tests'
    if (-not (Test-Path 'tests/Host/test_host.exe')) {
        g++ -O3 -std=c++17 -Isrc tests/Host/test_host.cpp src/ArcadeSimulation.cpp src/NativeSound.cpp src/Generated/MainProgram.cpp src/Generated/SoundProgram.cpp -o tests/Host/test_host.exe
        Assert-FroggerExit 'Build test host'
    }
    .\tests\Host\test_host.exe; Assert-FroggerExit 'Native and MAME fixture tests'
    node tools/verify_native.mjs; Assert-FroggerExit 'Full-state parity'
    node tools/audit.mjs; Assert-FroggerExit 'Coverage audit'
    & (Get-FroggerGodot) --headless --path godot --quit; Assert-FroggerExit 'Godot headless verification'
} finally { Pop-Location }
