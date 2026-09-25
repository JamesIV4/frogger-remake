. "$PSScriptRoot/tools/common.ps1"
Push-Location $FroggerRoot
try {
    if(-not (Test-Path godot/rom/maincpu.bin)){python tools/prepare_rom.py;Assert-FroggerExit 'ROM verification'}
    New-Item -ItemType Directory -Force builds/windows | Out-Null
    $froggerExportOut=Join-Path $FroggerRoot 'docs/evidence/export-stdout.log'
    $froggerExportErr=Join-Path $FroggerRoot 'docs/evidence/export-stderr.log'
    $froggerProject='"'+(Join-Path $FroggerRoot 'godot')+'"'
    $froggerExe='"'+(Join-Path $FroggerRoot 'builds/windows/Frogger Remake.exe')+'"'
    $froggerExport=Start-Process -FilePath (Get-FroggerGodot) -ArgumentList '--headless','--path',$froggerProject,'--export-release','"Windows Desktop"',$froggerExe -WindowStyle Hidden -RedirectStandardOutput $froggerExportOut -RedirectStandardError $froggerExportErr -Wait -PassThru
    if($froggerExport.ExitCode -ne 0 -or (Select-String -LiteralPath $froggerExportErr -Pattern '^ERROR:' -Quiet)){throw 'Export failed. See docs/evidence/export-stderr.log.'}
    New-Item -ItemType Directory -Force builds/windows/licenses | Out-Null
    Copy-Item -LiteralPath LICENSE,THIRD_PARTY.md -Destination builds/windows/licenses
    Get-ChildItem -LiteralPath godot/Fonts -Filter '*-OFL.txt' | Copy-Item -Destination builds/windows/licenses
    Write-Host 'Exported builds/windows/Frogger Remake.exe'
} finally { Pop-Location }
