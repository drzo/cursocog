# OpenCog Build Script for Windows
# This script builds all OpenCog components in the required order

# Define colors for output
$Green = [System.ConsoleColor]::Green
$Red = [System.ConsoleColor]::Red
$Yellow = [System.ConsoleColor]::Yellow
$Blue = [System.ConsoleColor]::Blue

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

function Section-Header($message) {
    Write-ColorOutput $Blue "`n=== $message ==="
}

# Get current directory and script path
$CURRENT_DIR = Get-Location
$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path

Write-ColorOutput $Blue "OpenCog Build System"
Write-ColorOutput $Blue "==================="

# Check for dependencies
Section-Header "Checking dependencies"
Write-Output "Verifying that all required dependencies are installed..."

# Run the dependency verification tool
$verifyScript = Join-Path -Path $SCRIPT_DIR -ChildPath "scripts\verify-dependencies.ps1"
if (Test-Path $verifyScript) {
    & $verifyScript
    if ($LASTEXITCODE -ne 0) {
        $confirm = Read-Host "Continue build despite dependency issues? (y/N)"
        if ($confirm -notmatch "^[yY]$") {
            Error-Exit "Build aborted due to dependency issues."
        }
        Write-ColorOutput $Yellow "Continuing despite dependency issues. The build may fail."
    } else {
        Write-ColorOutput $Green "All dependencies verified successfully."
    }
} else {
    Write-ColorOutput $Yellow "Dependency verification tool not found. Skipping dependency check."
    Write-ColorOutput $Yellow "You may encounter build failures if dependencies are missing."
    Write-Output "Run .\install-dependencies.ps1 to install required dependencies."
}

# Create build log directory
$LOGS_DIR = "build_logs"
if (!(Test-Path $LOGS_DIR)) {
    New-Item -ItemType Directory -Path $LOGS_DIR | Out-Null
}

# Function to check if a command exists
function Command-Exists($cmdname) {
    return [bool](Get-Command -Name $cmdname -ErrorAction SilentlyContinue)
}

# Function to build a component
function Build-Component($componentName, $componentDir) {
    $logFile = "$LOGS_DIR\${componentName}_build.log"
    
    Write-ColorOutput $Yellow "Building $componentName..."
    
    # Check if component directory exists
    if (!(Test-Path $componentDir)) {
        Write-ColorOutput $Red "Directory $componentDir not found. Cloning from GitHub..."
        git clone https://github.com/opencog/$componentName $componentDir
        if (!$?) {
            Error-Exit "Failed to clone $componentName repository."
        }
    }
    
    # Create build directory
    if (!(Test-Path "$componentDir\build")) {
        New-Item -ItemType Directory -Path "$componentDir\build" | Out-Null
    }
    
    # Configure, build and install
    Push-Location "$componentDir\build"
    
    Write-Output "Configuring $componentName..."
    cmake .. | Out-File -FilePath $logFile
    if (!$?) {
        Pop-Location
        Error-Exit "Failed to configure $componentName. See $logFile for details."
    }
    
    Write-Output "Building $componentName..."
    cmake --build . --config Release | Out-File -FilePath $logFile -Append
    if (!$?) {
        Pop-Location
        Error-Exit "Failed to build $componentName. See $logFile for details."
    }
    
    Write-Output "Installing $componentName..."
    cmake --install . | Out-File -FilePath $logFile -Append
    if (!$?) {
        Pop-Location
        Error-Exit "Failed to install $componentName. See $logFile for details."
    }
    
    Pop-Location
    
    Write-ColorOutput $Green "Successfully built and installed $componentName."
}

# Function to check prerequisites
function Check-Prerequisites {
    Write-Output "Checking prerequisites..."
    
    $missingTools = @()
    
    # Check for essential build tools
    foreach ($cmd in @("git", "cmake")) {
        if (!(Command-Exists $cmd)) {
            $missingTools += $cmd
        }
    }
    
    # Check for Visual Studio
    if (!(Get-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\VisualStudio\*" -ErrorAction SilentlyContinue)) {
        Write-ColorOutput $Yellow "Warning: Visual Studio may not be installed. Make sure you have Visual Studio with C++ development tools installed."
    }
    
    # Check for vcpkg (package manager)
    if (!(Command-Exists "vcpkg")) {
        Write-ColorOutput $Yellow "Warning: vcpkg not found. Consider installing it for managing dependencies."
        Write-ColorOutput $Yellow "Visit https://github.com/Microsoft/vcpkg for installation instructions."
    }
    
    if ($missingTools.Count -gt 0) {
        Write-ColorOutput $Red "The following required tools are missing:"
        foreach ($tool in $missingTools) {
            Write-ColorOutput $Red "  - $tool"
        }
        Error-Exit "Please install the missing tools and try again."
    }
    
    Write-ColorOutput $Green "Prerequisites check completed."
}

# Main build process
function Build-All {
    Write-Output "OpenCog Build System"
    Write-Output "===================="
    Write-Output ""
    
    # Check prerequisites
    Check-Prerequisites
    
    # Build components in order
    Build-Component "cogutil" "cogutil"
    Build-Component "atomspace" "atomspace"
    Build-Component "unify" "unify"
    Build-Component "ure" "ure"
    Build-Component "atomspace-storage" "atomspace-storage"
    Build-Component "atomspace-rocks" "atomspace-rocks"
    Build-Component "atomspace-pgres" "atomspace-pgres"
    Build-Component "cogserver" "cogserver"
    Build-Component "attention" "attention"
    Build-Component "spacetime" "spacetime"
    Build-Component "pln" "pln"
    Build-Component "lg-atomese" "lg-atomese"
    Build-Component "learn" "learn"
    Build-Component "opencog" "opencog"
    Build-Component "sensory" "sensory"
    Build-Component "evidence" "evidence"
    
    Write-ColorOutput $Green "All components have been built and installed successfully!"
}

# Parse command line arguments
param(
    [switch]$CheckOnly,
    [string]$Component,
    [switch]$Help
)

if ($Help) {
    Write-Output "Usage: .\build.ps1 [-CheckOnly] [-Component <name>] [-Help]"
    Write-Output "Build OpenCog components."
    Write-Output ""
    Write-Output "Options:"
    Write-Output "  -CheckOnly      Check prerequisites only without building"
    Write-Output "  -Component      Build a specific component"
    Write-Output "  -Help           Display this help and exit"
    Write-Output ""
    exit 0
}

if ($CheckOnly) {
    Check-Prerequisites
    exit 0
}

if ($Component) {
    Build-Component $Component $Component
    exit 0
}

Build-All 