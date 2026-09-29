. "$PSScriptRoot/tools/common.ps1"
Push-Location $FroggerRoot
try {
    if(-not (Test-Path godot/rom/maincpu.bin)){python tools/prepare_rom.py;Assert-FroggerExit 'ROM verification'}
    New-Item -ItemType Directory -Force builds/windows | Out-Null
    $froggerExportOut=Join-Path $FroggerRoot 'docs/evidence/export-stdout.log'
    $froggerExportErr=Join-Path $FroggerRoot 'docs/evidence/export-stderr.log'
    $froggerProject='"'+(Join-Path $FroggerRoot 'godot')+'"'
    $froggerDefault=Join-Path $FroggerRoot 'builds/windows/Frogger Remake.exe'
    $froggerTarget=$froggerDefault
    $froggerRunning=Get-CimInstance Win32_Process -Filter "Name = 'Frogger Remake.exe'" -ErrorAction SilentlyContinue | Where-Object { $_.ExecutablePath -eq $froggerDefault }
    if($froggerRunning){
        $froggerTarget=Join-Path $FroggerRoot ('builds/windows/Frogger Remake - '+(Get-Date -Format 'yyyyMMdd-HHmmss')+'.exe')
        Write-Host 'The existing game is running; exporting this build alongside it.'
    }
    $froggerExe='"'+$froggerTarget+'"'
    $froggerExport=Start-Process -FilePath (Get-FroggerGodot) -ArgumentList '--headless','--path',$froggerProject,'--export-release','"Windows Desktop"',$froggerExe -WindowStyle Hidden -RedirectStandardOutput $froggerExportOut -RedirectStandardError $froggerExportErr -Wait -PassThru
    if($froggerExport.ExitCode -ne 0 -or (Select-String -LiteralPath $froggerExportErr -Pattern '^ERROR:' -Quiet)){throw 'Export failed. See docs/evidence/export-stderr.log.'}
    New-Item -ItemType Directory -Force builds/windows/licenses | Out-Null
    Copy-Item -LiteralPath LICENSE,THIRD_PARTY.md -Destination builds/windows/licenses
    Get-ChildItem -LiteralPath godot/Fonts -Filter '*-OFL.txt' | Copy-Item -Destination builds/windows/licenses
    python tools/package_desktop.py --exe $froggerTarget --output builds/windows
    Assert-FroggerExit 'Compress desktop build'
    Write-Host "Exported $froggerTarget"
} finally { Pop-Location }
