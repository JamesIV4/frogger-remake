. "$PSScriptRoot/common.ps1"
Push-Location $FroggerRoot
try {
    $ghidraRoot=$env:GHIDRA_HOME
    if(-not $ghidraRoot){$ghidraRoot=Join-Path $env:USERPROFILE 'scoop/apps/ghidra/current'}
    $headless=Join-Path $ghidraRoot 'support/analyzeHeadless.bat'
    if(-not (Test-Path -LiteralPath $headless)){throw 'Set GHIDRA_HOME to Ghidra 12.1.2 or later.'}
    python tools/prepare_rom.py;Assert-FroggerExit 'ROM verification'
    node tools/ghidra/seeds.mjs;Assert-FroggerExit 'Main entry seeds'
    New-Item -ItemType Directory -Force reference/ghidra | Out-Null
    foreach($cpu in @('main','audio')){
        $image=if($cpu -eq 'main'){'maincpu'}else{'audiocpu'}
        $seeds=if($cpu -eq 'main'){'tools/ghidra/seeds.tsv'}else{'tools/ghidra/seeds-audio.tsv'}
        $log="docs/evidence/ghidra-$cpu.log"
        & $headless reference/ghidra "Frogger-$cpu" -import "reference/assembled/$image.bin" -processor z80:LE:16:default -cspec default -loader BinaryLoader -scriptPath tools/ghidra -postScript ExportFrogger.java "docs/evidence/ghidra-$cpu" $seeds $cpu -overwrite *> $log
        Assert-FroggerExit "Ghidra $cpu"
        # analyzeHeadless can exit 0 on a failed Java postScript. Check its own completion marker.
        if(-not (Select-String -LiteralPath $log -Pattern 'FROGGER: \d+/\d+ decompiled' -Quiet)){throw "Ghidra postScript did not finish; inspect $log"}
        if(Select-String -LiteralPath $log -Pattern 'ERROR REPORT SCRIPT ERROR' -Quiet){throw "Ghidra script failed; inspect $log"}
    }
    node tools/audit.mjs;Assert-FroggerExit 'Coverage audit'
} finally { Pop-Location }
