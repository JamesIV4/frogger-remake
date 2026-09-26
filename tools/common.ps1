$ErrorActionPreference = 'Stop'
$FroggerRoot = Split-Path -Parent $PSScriptRoot
function Get-FroggerGodot {
    if ($env:GODOT_EXE -and (Test-Path -LiteralPath $env:GODOT_EXE)) { return $env:GODOT_EXE }
    $froggerGodotCommand = Get-Command Godot*console.exe -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($froggerGodotCommand) { return $froggerGodotCommand.Source }
    $froggerCandidates = Get-ChildItem -Path 'C:\GameDev\Godot*' -Directory -ErrorAction SilentlyContinue
    foreach ($candidate in $froggerCandidates) {
        $executable = Get-ChildItem -LiteralPath $candidate.FullName -Filter '*console.exe' -File | Select-Object -First 1
        if ($executable) { return $executable.FullName }
    }
    throw 'Set GODOT_EXE to your Godot 4.7 console executable.'
}
function Assert-FroggerExit([string]$Step) { if ($LASTEXITCODE -ne 0) { throw "$Step failed (exit $LASTEXITCODE)." } }
