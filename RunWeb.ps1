param(
    [int]$Port = 8080,
    [switch]$NoBrowser
)
. "$PSScriptRoot/tools/common.ps1"
Push-Location $FroggerRoot
try {
    if (-not (Test-Path builds/web/index.html)) {
        Write-Host "Web build not found. Running BuildWeb.ps1..."
        & "$PSScriptRoot/BuildWeb.ps1"
    }
    $url = "http://localhost:$Port/index.html"
    if (-not $NoBrowser) {
        Write-Host "Opening $url in default browser..."
        Start-Process $url
    }
    Write-Host "Serving builds/web on $url (Press Ctrl+C to stop)..."
    python tools/serve_web.py $Port
} finally { Pop-Location }
