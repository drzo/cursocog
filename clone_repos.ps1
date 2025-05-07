# OpenCog Repository Cloner
# This script clones all necessary OpenCog repositories for development

Write-Host "OpenCog Repository Cloner" -ForegroundColor Cyan
Write-Host "=========================" -ForegroundColor Cyan
Write-Host ""

# Define console colors
$Green = [System.ConsoleColor]::Green
$Red = [System.ConsoleColor]::Red
$Yellow = [System.ConsoleColor]::Yellow

function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    else {
        $input | Write-Output
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

function Error-Exit($message) {
    Write-ColorOutput $Red $message
    exit 1
}

# Create logs directory
$LOGS_DIR = "clone_logs"
if (!(Test-Path $LOGS_DIR)) {
    New-Item -ItemType Directory -Path $LOGS_DIR | Out-Null
}

# Function to clone a repository
function Clone-Repo {
    param (
        [string]$RepoUrl,
        [string]$Branch = "master",
        [string]$DestDir = ""
    )
    
    if ([string]::IsNullOrEmpty($DestDir)) {
        # Extract repository name from URL
        $DestDir = [System.IO.Path]::GetFileNameWithoutExtension($RepoUrl.Split('/')[-1])
    }
    
    Write-ColorOutput $Yellow "Cloning $RepoUrl ($Branch) into $DestDir..."
    
    if (Test-Path $DestDir) {
        Write-ColorOutput $Yellow "$DestDir already exists. Updating..."
        $currentLocation = Get-Location
        Set-Location $DestDir
        
        # Redirect output to log file
        $logFile = Join-Path -Path $currentLocation -ChildPath "$LOGS_DIR\${DestDir}_clone.log"
        git pull origin $Branch > $logFile 2>&1
        
        if ($LASTEXITCODE -ne 0) {
            Set-Location $currentLocation
            Error-Exit "Failed to update $DestDir"
        }
        
        Set-Location $currentLocation
    }
    else {
        # Redirect output to log file
        $logFile = Join-Path -Path (Get-Location) -ChildPath "$LOGS_DIR\${DestDir}_clone.log"
        git clone --branch $Branch $RepoUrl $DestDir > $logFile 2>&1
        
        if ($LASTEXITCODE -ne 0) {
            Error-Exit "Failed to clone $RepoUrl"
        }
    }
    
    Write-ColorOutput $Green "Successfully cloned/updated $DestDir"
}

# Check if repos.txt exists
if (!(Test-Path "repos.txt")) {
    Error-Exit "repos.txt file not found!"
}

Write-Host "Cloning repositories from repos.txt..."
$repoLines = Get-Content "repos.txt"

foreach ($line in $repoLines) {
    # Skip comments and empty lines
    if ($line.StartsWith("#") -or [string]::IsNullOrWhiteSpace($line)) {
        continue
    }
    
    # Parse line: URL [BRANCH]
    $parts = $line.Trim() -split '\s+', 2
    $repoUrl = $parts[0]
    $branch = if ($parts.Length -gt 1) { $parts[1] } else { "master" }
    
    # Clone the repository
    Clone-Repo -RepoUrl $repoUrl -Branch $branch
}

Write-Host ""
Write-ColorOutput $Green "All repositories cloned successfully!"
Write-Host "See $LOGS_DIR\ for detailed clone logs." 