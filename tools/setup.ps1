param([switch]$RebuildModels)
. "$PSScriptRoot/common.ps1"
Push-Location $FroggerRoot
try {
    python tools/prepare_rom.py; Assert-FroggerExit 'ROM verification'
    python tools/recompile.py; Assert-FroggerExit 'Native generation'
    if ($RebuildModels) {
        $froggerBlender = $env:BLENDER_EXE
        if (-not $froggerBlender) { $froggerBlender = (Get-ChildItem 'C:\Program Files\Blender Foundation\Blender*\blender.exe' | Sort-Object FullName -Descending | Select-Object -First 1).FullName }
        if (-not $froggerBlender) { throw 'Set BLENDER_EXE to Blender 5.1 or later.' }
        & $froggerBlender --background --python art/scripts/build_assets.py; Assert-FroggerExit 'Blender export'
    }
    $froggerGodot = Get-FroggerGodot
    & $froggerGodot --headless --editor --path godot --import --quit; Assert-FroggerExit 'Godot import'
    dotnet build 'godot/Frogger Remake.csproj' -c Debug; Assert-FroggerExit 'Native build'
    Write-Host 'Ready. Open godot/project.godot and press F5, or run ./RunGame.ps1.'
} finally { Pop-Location }
