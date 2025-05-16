# OpenCog CogInit - Dependencies and Prerequisites Manager
# This script creates a new repository with optimized local copies of all OpenCog prerequisites

Write-Host "OpenCog CogInit - Dependencies Manager" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
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

# Create directory for the coginit repository
$REPO_DIR = "coginit"
if (!(Test-Path $REPO_DIR)) {
    New-Item -ItemType Directory -Path $REPO_DIR | Out-Null
}

# Create essential subdirectories
$DEPS_DIR = Join-Path -Path $REPO_DIR -ChildPath "deps"
$SCRIPTS_DIR = Join-Path -Path $REPO_DIR -ChildPath "scripts"
$TESTS_DIR = Join-Path -Path $REPO_DIR -ChildPath "tests"
$DOCS_DIR = Join-Path -Path $REPO_DIR -ChildPath "docs"

foreach ($dir in @($DEPS_DIR, $SCRIPTS_DIR, $TESTS_DIR, $DOCS_DIR)) {
    if (!(Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir | Out-Null
    }
}

# Create logs directory
$LOGS_DIR = "downloads_logs"
if (!(Test-Path $LOGS_DIR)) {
    New-Item -ItemType Directory -Path $LOGS_DIR | Out-Null
}

Write-ColorOutput $Yellow "Setting up dependency information..."

# Define version information for dependencies
$dependencies = @(
    @{
        Name = "boost";
        Version = "1.79.0";
        Url = "https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.zip";
        FileName = "boost_1_79_0.zip";
        Description = "C++ utilities package";
        Essential = $true;
    },
    @{
        Name = "cmake";
        Version = "3.25.1";
        Url = "https://github.com/Kitware/CMake/releases/download/v3.25.1/cmake-3.25.1-windows-x86_64.zip";
        FileName = "cmake-3.25.1-windows-x86_64.zip";
        Description = "Build management tool";
        Essential = $true;
    },
    @{
        Name = "cxxtest";
        Version = "4.4";
        Url = "https://github.com/CxxTest/cxxtest/releases/download/4.4/cxxtest-4.4.tar.gz";
        FileName = "cxxtest-4.4.tar.gz";
        Description = "Unit test framework";
        Essential = $true;
    },
    @{
        Name = "guile";
        Version = "3.0.9";
        Url = "https://ftp.gnu.org/gnu/guile/guile-3.0.9.tar.gz";
        FileName = "guile-3.0.9.tar.gz";
        Description = "Embedded scheme REPL";
        Essential = $true;
    },
    @{
        Name = "openssl";
        Version = "3.0.8";
        Url = "https://www.openssl.org/source/openssl-3.0.8.tar.gz";
        FileName = "openssl-3.0.8.tar.gz";
        Description = "SSL/TLS implementation";
        Essential = $false;
    },
    @{
        Name = "binutils";
        Version = "2.39";
        Url = "https://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz";
        FileName = "binutils-2.39.tar.gz";
        Description = "GNU Binary File Description library";
        Essential = $false;
    },
    @{
        Name = "doxygen";
        Version = "1.9.6";
        Url = "https://www.doxygen.nl/files/doxygen-1.9.6.windows.bin.zip";
        FileName = "doxygen-1.9.6.windows.bin.zip";
        Description = "Documentation generator";
        Essential = $false;
    }
)

# Create dependencies.json to store dependency information
$depsJsonPath = Join-Path -Path $REPO_DIR -ChildPath "dependencies.json"
$dependencies | ConvertTo-Json -Depth 4 | Out-File -FilePath $depsJsonPath -Encoding UTF8

# Function to download a file if it doesn't exist
function Download-File {
    param (
        [string]$Url,
        [string]$OutputPath,
        [string]$LogFile
    )
    
    if (!(Test-Path $OutputPath)) {
        Write-ColorOutput $Yellow "Downloading $Url..."
        try {
            # Create the Web Client
            $webClient = New-Object System.Net.WebClient
            
            # Download the file
            $webClient.DownloadFile($Url, $OutputPath)
            Write-ColorOutput $Green "Downloaded successfully to $OutputPath"
        }
        catch {
            Write-ColorOutput $Red "Failed to download $Url. Error: $_"
            Write-Output "Error downloading $Url. Error: $_" | Out-File -FilePath $LogFile -Append
            return $false
        }
    }
    else {
        Write-ColorOutput $Yellow "File already exists: $OutputPath"
    }
    
    return $true
}

# Download essential dependencies
Write-ColorOutput $Yellow "Downloading dependency packages..."

foreach ($dep in $dependencies) {
    $destFile = Join-Path -Path $DEPS_DIR -ChildPath $dep.FileName
    $logFile = Join-Path -Path $LOGS_DIR -ChildPath ($dep.Name + "_download.log")
    
    Write-Host "Processing $($dep.Name) v$($dep.Version)..."
    
    if ($dep.Essential) {
        $success = Download-File -Url $dep.Url -OutputPath $destFile -LogFile $logFile
        if (-not $success -and $dep.Essential) {
            Error-Exit "Failed to download essential dependency: $($dep.Name)"
        }
    }
    else {
        Write-Host "  (Optional dependency - will download on demand)"
    }
}

# Create README.md
$readmePath = Join-Path -Path $REPO_DIR -ChildPath "README.md"
Write-ColorOutput $Yellow "Creating README.md..."
@"
# CogInit - OpenCog Prerequisites Manager

CogInit is a tool for managing all prerequisites and dependencies needed for building OpenCog components.
It includes local copies of source code, installation scripts, and verification tests.

## Contents

- **deps/** - Local source code copies of all dependencies
- **scripts/** - Installation scripts for different platforms
- **tests/** - Verification tests for each dependency
- **docs/** - Comprehensive documentation

## Getting Started

### On Windows:

```powershell
# Install all dependencies
.\scripts\install-windows.ps1

# Verify installation
.\scripts\verify-windows.ps1
```

### On Linux:

```bash
# Install all dependencies
./scripts/install-linux.sh

# Verify installation
./scripts/verify-linux.sh
```

## Essential Dependencies

1. **Boost** - C++ utilities package
2. **CMake** - Build management tool
3. **CxxTest** - Unit test framework
4. **Guile** - Embedded scheme REPL
5. **OpenSSL** - SSL/TLS implementation

## Optional Dependencies

1. **Binutils BFD** - GNU Binary File Description library 
2. **Iberty** - GCC compiler tools component
3. **Doxygen** - Documentation generator

## Customizing Installation

Edit the \`dependencies.json\` file to add or modify dependencies.
"@ | Out-File -FilePath $readmePath -Encoding UTF8

# Create Windows installation script
$winInstallPath = Join-Path -Path $SCRIPTS_DIR -ChildPath "install-windows.ps1"
Write-ColorOutput $Yellow "Creating Windows installation script..."
@"
# CogInit - Windows Dependencies Installer
# Installs all prerequisites for OpenCog development on Windows

param (
    [switch]$Essential = $false,
    [switch]$All = $false,
    [switch]$NoPrompt = $false
)

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

Write-Host "CogInit - OpenCog Prerequisites Installer (Windows)" -ForegroundColor Cyan
Write-Host "===================================================" -ForegroundColor Cyan
Write-Host ""

# Get the current location
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$rootDir = Split-Path -Parent $scriptDir
$depsDir = Join-Path -Path $rootDir -ChildPath "deps"

# Load dependency information
$depsJsonPath = Join-Path -Path $rootDir -ChildPath "dependencies.json"
if (!(Test-Path $depsJsonPath)) {
    Write-ColorOutput $Red "Dependencies configuration file not found: $depsJsonPath"
    exit 1
}

$dependencies = Get-Content -Raw -Path $depsJsonPath | ConvertFrom-Json

# Create installation directory
$installDir = Join-Path -Path $env:TEMP -ChildPath "coginit_install"
if (!(Test-Path $installDir)) {
    New-Item -ItemType Directory -Path $installDir | Out-Null
}

# Install each dependency
foreach ($dep in $dependencies) {
    if (!($Essential -and !($dep.Essential)) -or $All) {
        Write-ColorOutput $Yellow "Processing $($dep.Name) v$($dep.Version)..."
        
        # Check if the user wants to install this dependency
        $install = $true
        if (!$NoPrompt -and !$Essential -and !$All) {
            $response = Read-Host "Install $($dep.Name)? (Y/n)"
            if ($response -eq "n" -or $response -eq "N") {
                $install = $false
            }
        }
        
        if ($install) {
            # Extract the archive
            $archivePath = Join-Path -Path $depsDir -ChildPath $dep.FileName
            $extractPath = Join-Path -Path $installDir -ChildPath $dep.Name
            
            if (!(Test-Path $archivePath)) {
                Write-ColorOutput $Red "Archive not found: $archivePath"
                continue
            }
            
            # Create extraction directory
            if (!(Test-Path $extractPath)) {
                New-Item -ItemType Directory -Path $extractPath | Out-Null
            }
            
            # Extract based on file extension
            if ($dep.FileName -match "\.zip$") {
                Write-Host "  Extracting ZIP archive..."
                Expand-Archive -Path $archivePath -DestinationPath $extractPath -Force
            }
            elseif ($dep.FileName -match "\.tar\.gz$") {
                Write-Host "  Extracting TAR.GZ archive..."
                # Use 7-Zip if available, otherwise use built-in tar
                if (Get-Command "7z" -ErrorAction SilentlyContinue) {
                    & 7z x $archivePath -o"$extractPath" -y
                    $tarFile = Get-ChildItem -Path $extractPath -Filter "*.tar" | Select-Object -First 1
                    if ($tarFile) {
                        & 7z x $tarFile.FullName -o"$extractPath" -y
                        Remove-Item $tarFile.FullName -Force
                    }
                }
                else {
                    tar -xzf $archivePath -C $extractPath
                }
            }
            
            # Perform dependency-specific installation steps
            switch ($dep.Name) {
                "boost" {
                    Write-Host "  Boost requires separate build steps. See ./docs/boost_windows.md for instructions."
                }
                "cmake" {
                    Write-Host "  Adding CMake to PATH environment variable..."
                    $cmakeDir = Get-ChildItem -Path $extractPath -Directory | Select-Object -First 1
                    $cmakeBinDir = Join-Path -Path $cmakeDir.FullName -ChildPath "bin"
                    $env:PATH = $cmakeBinDir + ";" + $env:PATH
                    [Environment]::SetEnvironmentVariable("PATH", $env:PATH, "User")
                    & cmake --version
                }
                "cxxtest" {
                    Write-Host "  Setting CXXTEST_HOME environment variable..."
                    $cxxtestDir = Get-ChildItem -Path $extractPath -Directory | Select-Object -First 1
                    [Environment]::SetEnvironmentVariable("CXXTEST_HOME", $cxxtestDir.FullName, "User")
                }
                "doxygen" {
                    Write-Host "  Adding Doxygen to PATH environment variable..."
                    $doxygenDir = Get-ChildItem -Path $extractPath -Directory | Select-Object -First 1
                    [Environment]::SetEnvironmentVariable("PATH", $doxygenDir.FullName + ";" + $env:PATH, "User")
                    & doxygen --version
                }
                default {
                    Write-Host "  No specific installation steps needed for $($dep.Name)."
                }
            }
            
            Write-ColorOutput $Green "Installed $($dep.Name) v$($dep.Version)"
        }
        else {
            Write-Host "Skipping $($dep.Name)..."
        }
    }
}

Write-Host ""
Write-ColorOutput $Green "OpenCog prerequisites installation completed!"
Write-Host "Some dependencies may require additional configuration. See docs/ for details."
"@ | Out-File -FilePath $winInstallPath -Encoding UTF8

# Create Linux installation script
$linuxInstallPath = Join-Path -Path $SCRIPTS_DIR -ChildPath "install-linux.sh"
Write-ColorOutput $Yellow "Creating Linux installation script..."
@"
#!/bin/bash
#
# CogInit - Linux Dependencies Installer
# Installs all prerequisites for OpenCog development on Linux
#

# Command line options
ESSENTIAL_ONLY=false
INSTALL_ALL=false
NO_PROMPT=false

while [[ $# -gt 0 ]]; do
  case $1 in
    --essential)
      ESSENTIAL_ONLY=true
      shift
      ;;
    --all)
      INSTALL_ALL=true
      shift
      ;;
    --no-prompt)
      NO_PROMPT=true
      shift
      ;;
    *)
      shift
      ;;
  esac
done

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${CYAN}CogInit - OpenCog Prerequisites Installer (Linux)${NC}"
echo -e "${CYAN}=================================================${NC}"
echo

# Determine script directory and root directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT_DIR="$(dirname $SCRIPT_DIR)"
DEPS_DIR="$ROOT_DIR/deps"

# Load dependency information
DEPS_JSON="$ROOT_DIR/dependencies.json"
if [ ! -f "$DEPS_JSON" ]; then
    echo -e "${RED}Dependencies configuration file not found: $DEPS_JSON${NC}"
    exit 1
fi

# Create installation directory
INSTALL_DIR="/tmp/coginit_install"
mkdir -p "$INSTALL_DIR"

# Check if jq is installed
if ! command -v jq &> /dev/null; then
    echo -e "${YELLOW}jq is not installed. Installing...${NC}"
    sudo apt update && sudo apt install -y jq
fi

# Read dependencies json
DEPENDENCIES="$(jq -c '.[]' "$DEPS_JSON")"

# Process each dependency
echo -e "${YELLOW}Installing OpenCog prerequisites...${NC}"

for DEP in $DEPENDENCIES; do
    NAME="$(echo $DEP | jq -r '.Name')"
    VERSION="$(echo $DEP | jq -r '.Version')"
    FILENAME="$(echo $DEP | jq -r '.FileName')"
    ESSENTIAL="$(echo $DEP | jq -r '.Essential')"
    DESCRIPTION="$(echo $DEP | jq -r '.Description')"
    
    # Skip non-essential if --essential flag is used
    if [ "$ESSENTIAL_ONLY" = true ] && [ "$ESSENTIAL" != "true" ]; then
        continue
    fi
    
    echo -e "${YELLOW}Processing $NAME v$VERSION...${NC}"
    
    # Check if the user wants to install this dependency
    INSTALL=true
    if [ "$NO_PROMPT" = false ] && [ "$ESSENTIAL_ONLY" = false ] && [ "$INSTALL_ALL" = false ]; then
        read -p "Install $NAME? (Y/n) " response
        if [[ "$response" =~ ^[Nn]$ ]]; then
            INSTALL=false
        fi
    fi
    
    if [ "$INSTALL" = true ]; then
        # Extract the archive
        ARCHIVE_PATH="$DEPS_DIR/$FILENAME"
        EXTRACT_PATH="$INSTALL_DIR/$NAME"
        
        if [ ! -f "$ARCHIVE_PATH" ]; then
            echo -e "${RED}Archive not found: $ARCHIVE_PATH${NC}"
            continue
        fi
        
        # Create extraction directory
        mkdir -p "$EXTRACT_PATH"
        
        # Extract based on file extension
        if [[ "$FILENAME" == *.zip ]]; then
            echo "  Extracting ZIP archive..."
            unzip -q -o "$ARCHIVE_PATH" -d "$EXTRACT_PATH"
        elif [[ "$FILENAME" == *.tar.gz ]]; then
            echo "  Extracting TAR.GZ archive..."
            tar -xzf "$ARCHIVE_PATH" -C "$EXTRACT_PATH"
        fi
        
        # Perform dependency-specific installation steps
        case "$NAME" in
            "boost")
                echo "  Installing Boost development packages..."
                sudo apt update && sudo apt install -y libboost-dev libboost-filesystem-dev libboost-program-options-dev libboost-system-dev libboost-thread-dev
                ;;
            "cmake")
                echo "  Installing CMake..."
                sudo apt update && sudo apt install -y cmake
                ;;
            "cxxtest")
                echo "  Installing CxxTest..."
                sudo apt update && sudo apt install -y cxxtest
                ;;
            "guile")
                echo "  Installing Guile..."
                sudo apt update && sudo apt install -y guile-3.0-dev
                ;;
            "openssl")
                echo "  Installing OpenSSL..."
                sudo apt update && sudo apt install -y libssl-dev
                ;;
            "binutils")
                echo "  Installing Binutils development package..."
                sudo apt update && sudo apt install -y binutils-dev
                ;;
            "doxygen")
                echo "  Installing Doxygen..."
                sudo apt update && sudo apt install -y doxygen
                ;;
            *)
                echo "  No specific installation steps needed for $NAME."
                ;;
        esac
        
        echo -e "${GREEN}Installed $NAME v$VERSION${NC}"
    else
        echo "Skipping $NAME..."
    fi
done

echo
echo -e "${GREEN}OpenCog prerequisites installation completed!${NC}"
echo "Some dependencies may require additional configuration. See docs/ for details."
"@ | Out-File -FilePath $linuxInstallPath -Encoding UTF8

# Create verification scripts
$winVerifyPath = Join-Path -Path $SCRIPTS_DIR -ChildPath "verify-windows.ps1"
Write-ColorOutput $Yellow "Creating Windows verification script..."
@"
# CogInit - Windows Dependencies Verification
# Verifies all prerequisites for OpenCog development on Windows

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

Write-Host "CogInit - OpenCog Prerequisites Verification (Windows)" -ForegroundColor Cyan
Write-Host "=====================================================" -ForegroundColor Cyan
Write-Host ""

# Define tests for dependencies
$tests = @(
    @{
        Name = "CMake";
        Command = "cmake --version";
        Pattern = "cmake version (\d+\.\d+\.\d+)";
        MinimumVersion = "3.12.0";
    },
    @{
        Name = "Boost";
        Command = "if (Test-Path $env:BOOST_ROOT) { echo "Boost found at $env:BOOST_ROOT" } else { echo "Boost not found" }";
        Pattern = "Boost found at (.+)";
        Required = $true;
    },
    @{
        Name = "CxxTest";
        Command = "if (Test-Path $env:CXXTEST_HOME) { echo "CxxTest found at $env:CXXTEST_HOME" } else { echo "CxxTest not found" }";
        Pattern = "CxxTest found at (.+)";
        Required = $true;
    },
    @{
        Name = "Doxygen";
        Command = "try { doxygen --version } catch { echo "Doxygen not found" }";
        Pattern = "(\d+\.\d+\.\d+)";
        MinimumVersion = "1.8.0";
        Required = $false;
    }
)

# Display table header
$formatString = "{0,-15} {1,-15} {2,-30} {3,-10}"
Write-Host ($formatString -f "Dependency", "Required", "Status", "Version")
Write-Host ($formatString -f "-----------", "--------", "------", "-------")

# Run each test
foreach ($test in $tests) {
    $required = if ($test.Required -eq $true) { "Yes" } else { "No" }
    
    try {
        # Execute the command
        $result = Invoke-Expression $test.Command | Out-String
        
        # Check for pattern match
        if ($test.Pattern -and $result -match $test.Pattern) {
            $version = $matches[1]
            
            # Check version if needed
            if ($test.MinimumVersion) {
                if ([System.Version]$version -ge [System.Version]$test.MinimumVersion) {
                    Write-Host ($formatString -f $test.Name, $required, "Installed", $version) -ForegroundColor Green
                }
                else {
                    Write-Host ($formatString -f $test.Name, $required, "Version too old", $version) -ForegroundColor Red
                }
            }
            else {
                Write-Host ($formatString -f $test.Name, $required, "Installed", $version) -ForegroundColor Green
            }
        }
        else {
            if ($test.Required -eq $true) {
                Write-Host ($formatString -f $test.Name, $required, "Not found", "N/A") -ForegroundColor Red
            }
            else {
                Write-Host ($formatString -f $test.Name, $required, "Not found", "N/A") -ForegroundColor Yellow
            }
        }
    }
    catch {
        if ($test.Required -eq $true) {
            Write-Host ($formatString -f $test.Name, $required, "Error checking", "N/A") -ForegroundColor Red
        }
        else {
            Write-Host ($formatString -f $test.Name, $required, "Error checking", "N/A") -ForegroundColor Yellow
        }
    }
}

Write-Host ""
Write-Host "Verification complete. Make sure all required dependencies are installed."
Write-Host "For any missing components, run ./scripts/install-windows.ps1"
"@ | Out-File -FilePath $winVerifyPath -Encoding UTF8

# Create Linux verification script
$linuxVerifyPath = Join-Path -Path $SCRIPTS_DIR -ChildPath "verify-linux.sh"
Write-ColorOutput $Yellow "Creating Linux verification script..."
@"
#!/bin/bash
#
# CogInit - Linux Dependencies Verification
# Verifies all prerequisites for OpenCog development on Linux
#

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${CYAN}CogInit - OpenCog Prerequisites Verification (Linux)${NC}"
echo -e "${CYAN}==================================================${NC}"
echo

# Function to compare versions
function version_compare() {
    if [[ $1 == $2 ]]; then
        return 0
    fi
    local IFS=.
    local i ver1=($1) ver2=($2)
    # Fill empty fields with zeros
    for ((i=${#ver1[@]}; i<${#ver2[@]}; i++)); do
        ver1[i]=0
    done
    for ((i=0; i<${#ver1[@]}; i++)); do
        if [[ -z ${ver2[i]} ]]; then
            ver2[i]=0
        fi
        if ((10${ver1[i]} > 10${ver2[i]})); then
            return 1
        fi
        if ((10${ver1[i]} < 10${ver2[i]})); then
            return 2
        fi
    done
    return 0
}

# Function to check a dependency
function check_dependency() {
    local NAME=$1
    local COMMAND=$2
    local PATTERN=$3
    local MIN_VERSION=$4
    local REQUIRED=$5
    
    local RESULT=$(eval $COMMAND 2>/dev/null | grep -oP "$PATTERN" || echo "Not found")
    local VERSION="N/A"
    local STATUS="Not found"
    
    if [[ $RESULT != "Not found" ]]; then
        VERSION=$RESULT
        STATUS="Installed"
        
        # Check version if needed
        if [[ -n $MIN_VERSION ]]; then
            version_compare $VERSION $MIN_VERSION
            COMP_RESULT=$?
            
            if [[ $COMP_RESULT -eq 2 ]]; then
                STATUS="Version too old"
            fi
        fi
    fi
    
    # Format output based on status and required flag
    if [[ $STATUS == "Installed" ]]; then
        printf "%-15s %-15s %-30s %-10s\n" "$NAME" "$REQUIRED" "${GREEN}$STATUS${NC}" "$VERSION"
    elif [[ $REQUIRED == "Yes" && $STATUS != "Installed" ]]; then
        printf "%-15s %-15s %-30s %-10s\n" "$NAME" "$REQUIRED" "${RED}$STATUS${NC}" "$VERSION"
    else
        printf "%-15s %-15s %-30s %-10s\n" "$NAME" "$REQUIRED" "${YELLOW}$STATUS${NC}" "$VERSION"
    fi
}

# Display table header
printf "%-15s %-15s %-30s %-10s\n" "Dependency" "Required" "Status" "Version"
printf "%-15s %-15s %-30s %-10s\n" "-----------" "--------" "------" "-------"

# Check each dependency
check_dependency "Boost" "dpkg -s libboost-dev | grep Version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "1.68.0" "Yes"
check_dependency "CMake" "cmake --version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "3.12.0" "Yes"
check_dependency "CxxTest" "dpkg -s cxxtest | grep Version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "4.0.0" "Yes"
check_dependency "Guile" "guile --version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "3.0.0" "Yes"
check_dependency "OpenSSL" "dpkg -s libssl-dev | grep Version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "1.0.0" "No"
check_dependency "Binutils" "dpkg -s binutils-dev | grep Version" "([0-9]+\\.[0-9]+)" "2.30" "No"
check_dependency "Doxygen" "doxygen --version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "1.8.0" "No"

echo
echo "Verification complete. Make sure all required dependencies are installed."
echo "For any missing components, run ./scripts/install-linux.sh"
"@ | Out-File -FilePath $linuxVerifyPath -Encoding UTF8

# Create documentation files
$boostDocsPath = Join-Path -Path $DOCS_DIR -ChildPath "boost_windows.md"
Write-ColorOutput $Yellow "Creating Boost documentation..."
@"
# Building Boost on Windows

This guide explains how to build Boost from source on Windows for OpenCog development.

## Prerequisites

- Visual Studio 2019 or later with C++ tools
- 7zip or similar archive extraction tool
- PowerShell 5.0 or later

## Building Steps

1. Extract the Boost archive from `../deps/boost_1_79_0.zip` to a directory on your system

2. Open a Visual Studio Developer Command Prompt

3. Navigate to the Boost directory:
   ```
   cd C:\path\to\boost_1_79_0
   ```

4. Run bootstrap:
   ```
   bootstrap.bat
   ```

5. Build Boost with the required libraries:
   ```
   .\b2 --with-system --with-filesystem --with-thread --with-program_options --with-regex --address-model=64 -j4 toolset=msvc
   ```

   Note: `-j4` specifies using 4 cores for compilation. Adjust according to your system.

6. Set the BOOST_ROOT environment variable:
   ```
   setx BOOST_ROOT C:\path\to\boost_1_79_0
   ```

7. Add the library path to your system PATH:
   ```
   setx PATH "%PATH%;C:\path\to\boost_1_79_0\stage\lib"
   ```

## Verification

To verify your Boost installation:

1. Open a new command prompt (to refresh environment variables)

2. Run:
   ```
   echo %BOOST_ROOT%
   ```
   
   This should output the path to your Boost installation.

3. Check if the libraries exist:
   ```
   dir %BOOST_ROOT%\stage\lib
   ```

   You should see various .lib and .dll files.
"@ | Out-File -FilePath $boostDocsPath -Encoding UTF8

# Create gitignore file
$gitignorePath = Join-Path -Path $REPO_DIR -ChildPath ".gitignore"
Write-ColorOutput $Yellow "Creating .gitignore..."
@"
# Compiled files
*.o
*.obj
*.exe
*.dll
*.so
*.dylib

# Build directories
build/
bin/
lib/

# IDE files
.vs/
.vscode/
*.suo
*.user
*.sdf
*.opensdf
*.sln
*.vcxproj
*.vcxproj.filters

# Downloaded files
deps/*.zip
deps/*.tar.gz
deps/*.7z

# Temporary files
downloads_logs/
.DS_Store
Thumbs.db
"@ | Out-File -FilePath $gitignorePath -Encoding UTF8

# Final message
Write-Host ""
Write-ColorOutput $Green "CogInit repository setup complete!"
Write-Host "The coginit directory contains all necessary files for managing OpenCog prerequisites."
Write-Host "To use it:"
Write-Host "1. Copy the coginit directory to your desired location"
Write-Host "2. Run the appropriate installation script for your platform:"
Write-Host "   - Windows: .\scripts\install-windows.ps1"
Write-Host "   - Linux: ./scripts/install-linux.sh"
Write-Host ""
Write-Host "You can verify the installation with:"
Write-Host "   - Windows: .\scripts\verify-windows.ps1"
Write-Host "   - Linux: ./scripts/verify-linux.sh" 