$ErrorActionPreference = 'Stop'
$FroggerRoot = Split-Path -Parent $PSScriptRoot
function Get-FroggerGodot {
    if ($env:GODOT_EXE -and (Test-Path -LiteralPath $env:GODOT_EXE)) { return $env:GODOT_EXE }
    $froggerCandidates = Get-ChildItem -Path 'C:\GameDev\Godot*' -Directory -ErrorAction SilentlyContinue |
        Sort-Object { if ($_.Name -like '*mono*') { 1 } else { 0 } }
    foreach ($candidate in $froggerCandidates) {
        $executable = Get-ChildItem -LiteralPath $candidate.FullName -Filter '*console.exe' -File | Select-Object -First 1
        if ($executable) { return $executable.FullName }
    }
    $froggerGodotCommand = Get-Command Godot*console.exe -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($froggerGodotCommand) { return $froggerGodotCommand.Source }
    throw 'Set GODOT_EXE to your Godot 4.7 console executable.'
}
function Get-FroggerGodotStandard {
    if ($env:GODOT_STANDARD_EXE -and (Test-Path -LiteralPath $env:GODOT_STANDARD_EXE)) { return $env:GODOT_STANDARD_EXE }
    $froggerCandidates = Get-ChildItem -Path 'C:\GameDev\Godot*' -Directory -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -notlike '*mono*' }
    foreach ($candidate in $froggerCandidates) {
        $executable = Get-ChildItem -LiteralPath $candidate.FullName -Filter '*console.exe' -File | Select-Object -First 1
        if ($executable) { return $executable.FullName }
    }
    return Get-FroggerGodot
}
function Assert-FroggerExit([string]$Step) { if ($null -ne $LASTEXITCODE -and $LASTEXITCODE -ne 0) { throw "$Step failed (exit $LASTEXITCODE)." } }
