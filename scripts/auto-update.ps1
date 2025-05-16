# OpenCog Auto-Update PowerShell Wrapper
# This script provides a Windows-friendly wrapper for the auto-update.py script

param(
    [Parameter(Position=0, Mandatory=$false)]
    [ValidateSet("check", "update")]
    [string]$action = "check",
    
    [Parameter(Mandatory=$false)]
    [switch]$silent,
    
    [Parameter(Mandatory=$false)]
    [string]$log,
    
    [Parameter(Mandatory=$false)]
    [switch]$help,
    
    [Parameter(Mandatory=$false)]
    [switch]$version
)

# Script directory
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$rootDir = Split-Path -Parent $scriptDir

# Python script path
$pythonScript = Join-Path -Path $scriptDir -ChildPath "auto-update.py"

# Display help
function Show-Help {
    Write-Host "OpenCog Auto-Update System"
    Write-Host ""
    Write-Host "Usage:"
    Write-Host "  auto-update.ps1 [check|update] [options]"
    Write-Host ""
    Write-Host "Actions:"
    Write-Host "  check    Check for available updates (default)"
    Write-Host "  update   Download and install available updates"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -silent   Don't display progress information"
    Write-Host "  -log      Log file to write to"
    Write-Host "  -help     Show this help message"
    Write-Host "  -version  Show version information"
    exit 0
}

# Display version
function Show-Version {
    Write-Host "OpenCog Auto-Update System v1.0.0"
    exit 0
}

# Check if Python is available
function Check-Python {
    try {
        $pythonVersion = python --version
        return $true
    }
    catch {
        Write-Host "Python is not installed or not in the PATH"
        Write-Host "Please install Python 3.6 or later from https://www.python.org/"
        return $false
    }
}

# Main function
function Main {
    # Check for help flag
    if ($help) {
        Show-Help
        return
    }
    
    # Check for version flag
    if ($version) {
        Show-Version
        return
    }
    
    # Check for Python
    if (-not (Check-Python)) {
        exit 1
    }
    
    # Build command arguments
    $cmdArgs = @($pythonScript, $action)
    
    if ($silent) {
        $cmdArgs += "--silent"
    }
    
    if ($log) {
        $cmdArgs += "--log"
        $cmdArgs += $log
    }
    
    # Run the Python script
    try {
        Write-Host "Running $pythonScript $action..."
        & python $cmdArgs
        return $LASTEXITCODE
    }
    catch {
        Write-Host "Error running auto-update script: $_"
        return 1
    }
}

# Run the main function
exit (Main) 