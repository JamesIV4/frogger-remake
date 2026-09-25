. "$PSScriptRoot/common.ps1"
Push-Location $FroggerRoot
try {
    python tools/prepare_rom.py; Assert-FroggerExit 'ROM verification'
    python tools/validate_assets.py; Assert-FroggerExit 'Asset validation'
    node --test --test-concurrency=2 'tests/*.test.js' 'vendor/arcade-js/games/frogger/test/*.test.js' 'vendor/arcade-js/games/frogger/idiomatic/test/*.test.js' 'vendor/arcade-js/boards/frogger/test/*.test.js'; Assert-FroggerExit 'Recovered-function tests'
    dotnet run --project tests/Host/Host.csproj -c Release; Assert-FroggerExit 'Native and MAME fixture tests'
    node tools/verify_native.mjs; Assert-FroggerExit 'Full-state parity'
    node tools/audit.mjs; Assert-FroggerExit 'Coverage audit'
    dotnet build 'godot/Frogger Remake.csproj' -c Debug; Assert-FroggerExit 'Godot build'
} finally { Pop-Location }
