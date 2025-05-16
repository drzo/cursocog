# OpenCog Basic Components Cloner
# This script clones only the essential OpenCog repositories

Write-Host "OpenCog Basic Components Cloner" -ForegroundColor Cyan
Write-Host "===============================" -ForegroundColor Cyan
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

# Create directory for the standalone repository
$STANDALONE_DIR = "opencog-basic"
if (!(Test-Path $STANDALONE_DIR)) {
    New-Item -ItemType Directory -Path $STANDALONE_DIR | Out-Null
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
    
    Write-ColorOutput $Yellow "Cloning $RepoUrl ($Branch) into $STANDALONE_DIR\$DestDir..."
    
    $targetPath = Join-Path -Path $STANDALONE_DIR -ChildPath $DestDir
    
    if (Test-Path $targetPath) {
        Write-ColorOutput $Yellow "$DestDir already exists. Updating..."
        $currentLocation = Get-Location
        Set-Location $targetPath
        
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
        git clone --branch $Branch $RepoUrl $targetPath > $logFile 2>&1
        
        if ($LASTEXITCODE -ne 0) {
            Error-Exit "Failed to clone $RepoUrl"
        }
    }
    
    Write-ColorOutput $Green "Successfully cloned/updated $DestDir"
}

# Check if repos-basic.txt exists
if (!(Test-Path "repos-basic.txt")) {
    Error-Exit "repos-basic.txt file not found!"
}

Write-Host "Cloning basic components into $STANDALONE_DIR..."
$repoLines = Get-Content "repos-basic.txt"

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

# Copy essential build files to the standalone directory
Write-ColorOutput $Yellow "Copying build system files..."
try {
    Copy-Item -Path "install-dependencies.ps1", "build.ps1" -Destination $STANDALONE_DIR -ErrorAction SilentlyContinue
} catch {
    Write-Host "Warning: Some build files not found, skipping."
}

# Create a basic CMakeLists.txt if it doesn't exist
$cmakeListsPath = Join-Path -Path $STANDALONE_DIR -ChildPath "CMakeLists.txt"
if (!(Test-Path $cmakeListsPath)) {
    Write-ColorOutput $Yellow "Creating basic CMakeLists.txt..."
    @"
# OpenCog Basic Components
# CMake build file for minimal OpenCog setup

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(opencog-basic)

# Default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

# Add each component subdirectory
ADD_SUBDIRECTORY(cogutil)
ADD_SUBDIRECTORY(atomspace)
ADD_SUBDIRECTORY(atomspace-storage)
ADD_SUBDIRECTORY(atomspace-rocks)
ADD_SUBDIRECTORY(cogserver)

MESSAGE(STATUS "OpenCog basic components build configured")
"@ | Out-File -FilePath $cmakeListsPath -Encoding UTF8
}

# Create a basic README.md
$readmePath = Join-Path -Path $STANDALONE_DIR -ChildPath "README.md"
Write-ColorOutput $Yellow "Creating README.md..."
@"
# OpenCog Basic Components

This is a standalone repository containing only the essential core components of the OpenCog framework.

## Components Included

1. **cogutil** - Utility library with common functions and data structures
2. **atomspace** - Core knowledge representation framework
3. **atomspace-storage** - Persistence API for AtomSpace
4. **atomspace-rocks** - RocksDB backend for AtomSpace persistence
5. **cogserver** - Network server providing AtomSpace API access

## Building

### Prerequisites

- C++17 compatible compiler (gcc 7.5+, clang 6+, or MSVC 2019+)
- CMake 3.12+
- Boost 1.68+
- Python 3.6+ with development headers

### Linux/macOS Build

```bash
# Install dependencies
./install-dependencies.sh

# Build
./build.sh
```

### Windows Build

```powershell
# Install dependencies
./install-dependencies.ps1

# Build
./build.ps1
```

## Usage

After building, you can use the components individually or together as a cohesive system.

Basic example to start a CogServer:

```bash
./opencog-basic/cogserver/build/opencog/cogserver/server/cogserver
```

## Advanced Components

For advanced functionality, you may want to add additional components:
- atomspace-pgres - PostgreSQL backend for AtomSpace persistence
- unify - Pattern matching and unification
- ure - Rule engine for inference

These can be enabled by uncommenting the respective entries in repos-basic.txt
and running the clone script again.
"@ | Out-File -FilePath $readmePath -Encoding UTF8

Write-Host ""
Write-ColorOutput $Green "Basic OpenCog components cloned successfully to $STANDALONE_DIR!"
Write-Host "See $LOGS_DIR\ for detailed clone logs."
Write-Host ""
Write-ColorOutput $Yellow "Next steps:"
Write-Host "1. cd $STANDALONE_DIR"
Write-Host "2. .\install-dependencies.ps1  # Install prerequisites"
Write-Host "3. .\build.ps1                 # Build all components" 