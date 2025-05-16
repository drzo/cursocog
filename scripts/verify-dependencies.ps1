# OpenCog Dependency Verification Tool - PowerShell Wrapper
# This script runs the verify-dependencies.py Python script

# Check if Python is installed
$pythonCommand = $null
$pythonCommands = @("python", "python3")

foreach ($cmd in $pythonCommands) {
    if (Get-Command $cmd -ErrorAction SilentlyContinue) {
        $pythonCommand = $cmd
        break
    }
}

if ($null -eq $pythonCommand) {
    Write-Host "Python is not installed or not in PATH. Please install Python 3.6 or newer." -ForegroundColor Red
    Write-Host "You can download Python from: https://www.python.org/downloads/" -ForegroundColor Yellow
    exit 1
}

# Get script directory
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
$scriptFile = Join-Path -Path $scriptPath -ChildPath "verify-dependencies.py"
$rootDir = (Get-Item $scriptPath).Parent.FullName

# Run the verification script
Write-Host "Running OpenCog Dependency Verification Tool..." -ForegroundColor Cyan
try {
    & $pythonCommand $scriptFile $args
    $exitCode = $LASTEXITCODE
    
    # If there were issues, suggest running the installer
    if ($exitCode -ne 0) {
        Write-Host "`nTo automatically install missing dependencies, run:" -ForegroundColor Yellow
        Write-Host "    .\install-dependencies.ps1" -ForegroundColor Yellow
        Write-Host "Note: Run this command from an Administrator PowerShell prompt." -ForegroundColor Yellow
    }
    
    exit $exitCode
} catch {
    Write-Host "Error running verification script: $_" -ForegroundColor Red
    exit 1
} 