. "$PSScriptRoot/tools/common.ps1"
Push-Location $FroggerRoot
try {
    & (Get-FroggerGodot) --path godot
} finally { Pop-Location }
