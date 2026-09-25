. "$PSScriptRoot/tools/common.ps1"
Push-Location $FroggerRoot
try {
    dotnet build 'godot/Frogger Remake.csproj' -c Debug; Assert-FroggerExit 'Native build'
    & (Get-FroggerGodot) --path godot
} finally { Pop-Location }
