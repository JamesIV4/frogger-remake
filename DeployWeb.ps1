param(
    [string]$ProjectName = "frogger-remake",
    [string]$Branch = "main",
    [switch]$SkipBuild,
    [switch]$PushBranch
)

$ErrorActionPreference = 'Stop'
. "$PSScriptRoot/tools/common.ps1"

Push-Location $FroggerRoot
try {
    if (-not $SkipBuild) {
        Write-Host "Building web artifacts with BuildWeb.ps1..." -ForegroundColor Cyan
        & "$PSScriptRoot/BuildWeb.ps1"
        Assert-FroggerExit 'BuildWeb'
    }

    $webDir = Join-Path $FroggerRoot "builds/web"
    $indexHtml = Join-Path $webDir "index.html"
    $headersFile = Join-Path $webDir "_headers"

    if (-not (Test-Path -LiteralPath $indexHtml)) {
        throw "Web build not found at $indexHtml. Run BuildWeb.ps1 first."
    }

    if (-not (Test-Path -LiteralPath $headersFile)) {
        Write-Host "Creating _headers in $webDir..." -ForegroundColor Yellow
        $headersContent = @"
/*
  Cross-Origin-Opener-Policy: same-origin
  Cross-Origin-Embedder-Policy: require-corp
  Cross-Origin-Resource-Policy: same-origin
"@
        Set-Content -LiteralPath $headersFile -Value $headersContent -Encoding ascii
    }

    if ($PushBranch) {
        Write-Host "Publishing web build to branch 'web-release'..." -ForegroundColor Cyan
        $tempDir = Join-Path $env:TEMP ("frogger-deploy-" + [System.Guid]::NewGuid().ToString("N"))
        try {
            New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
            Copy-Item -Path "$webDir/*" -Destination $tempDir -Recurse -Force

            $originUrl = (git config --get remote.origin.url)
            if (-not $originUrl) {
                throw "No git remote 'origin' configured."
            }

            Push-Location $tempDir
            try {
                git init -b web-release | Out-Null
                git remote add origin $originUrl | Out-Null
                git add -A | Out-Null
                $commitMsg = "Deploy web build $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
                git commit -m $commitMsg | Out-Null
                Write-Host "Pushing to origin/web-release..." -ForegroundColor Cyan
                git push origin web-release --force
                Assert-FroggerExit 'Git push to web-release'
                Write-Host "Successfully pushed build artifacts to branch 'web-release'!" -ForegroundColor Green
            } finally {
                Pop-Location
            }
        } finally {
            if (Test-Path -LiteralPath $tempDir) {
                Remove-Item -LiteralPath $tempDir -Recurse -Force -ErrorAction SilentlyContinue
            }
        }
    } else {
        Write-Host "Deploying $webDir to Cloudflare Pages project '$ProjectName'..." -ForegroundColor Cyan
        
        $hasEnvAuth = [bool]($env:CLOUDFLARE_API_TOKEN)
        if (-not $hasEnvAuth) {
            $whoami = (& npx --yes wrangler whoami 2>&1 | Out-String)
            $isLoggedIn = ($whoami -match "You are logged in")
            if (-not $isLoggedIn) {
                Write-Host "Cloudflare authentication required." -ForegroundColor Yellow
                Write-Host "Please authenticate using one of the following methods:" -ForegroundColor Yellow
                Write-Host "  1. Run 'npx wrangler login' in your interactive terminal." -ForegroundColor Cyan
                Write-Host "  2. Set environment variables:" -ForegroundColor Cyan
                Write-Host "     `$env:CLOUDFLARE_API_TOKEN = '<your-api-token>'" -ForegroundColor White
                Write-Host "     `$env:CLOUDFLARE_ACCOUNT_ID = '<your-account-id>'" -ForegroundColor White
                Write-Host ""
            }
        }

        & npx --yes wrangler pages deploy "builds/web" --project-name $ProjectName --branch $Branch
        Assert-FroggerExit 'Cloudflare Pages deployment'

        Write-Host "Deployment completed successfully!" -ForegroundColor Green
    }
} finally {
    Pop-Location
}
