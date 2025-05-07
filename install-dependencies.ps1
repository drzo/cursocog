# OpenCog Dependencies Installer for Windows
# This script installs all dependencies required to build OpenCog on Windows

# Define colors for output
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

# Check for admin privileges
$currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Error-Exit "This script needs to be run as Administrator. Please restart PowerShell as Administrator and try again."
}

Write-Output "OpenCog Dependencies Installer for Windows"
Write-Output "==========================================="
Write-Output ""

# Check for git
if (-not (Get-Command "git" -ErrorAction SilentlyContinue)) {
    Write-ColorOutput $Yellow "Git not found. Installing Git..."
    # Download and install Git
    $gitInstaller = "$env:TEMP\Git-Install.exe"
    Invoke-WebRequest -Uri "https://github.com/git-for-windows/git/releases/download/v2.41.0.windows.1/Git-2.41.0-64-bit.exe" -OutFile $gitInstaller
    Start-Process -FilePath $gitInstaller -Args "/VERYSILENT /NORESTART" -Wait
    # Add Git to path
    $env:Path += ";C:\Program Files\Git\cmd"
}

# Check for Visual Studio
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vsWhere)) {
    Write-ColorOutput $Red "Visual Studio not found. Please install Visual Studio with C++ development tools."
    Write-ColorOutput $Red "Download from: https://visualstudio.microsoft.com/downloads/"
    Exit 1
}

$vsInstallPath = & $vsWhere -latest -property installationPath
if (-not $vsInstallPath) {
    Write-ColorOutput $Red "Visual Studio installation not found. Please install Visual Studio with C++ development tools."
    Exit 1
}

# Check for CMake
if (-not (Get-Command "cmake" -ErrorAction SilentlyContinue)) {
    Write-ColorOutput $Yellow "CMake not found. Installing CMake..."
    # Download and install CMake
    $cmakeInstaller = "$env:TEMP\cmake-install.msi"
    Invoke-WebRequest -Uri "https://github.com/Kitware/CMake/releases/download/v3.27.1/cmake-3.27.1-windows-x86_64.msi" -OutFile $cmakeInstaller
    Start-Process -FilePath "msiexec.exe" -ArgumentList "/i", $cmakeInstaller, "/quiet", "/norestart" -Wait
    # Add CMake to path
    $env:Path += ";C:\Program Files\CMake\bin"
}

# Install vcpkg
$vcpkgPath = "$env:USERPROFILE\vcpkg"
if (-not (Test-Path $vcpkgPath)) {
    Write-ColorOutput $Yellow "vcpkg not found. Installing vcpkg..."
    git clone https://github.com/Microsoft/vcpkg.git $vcpkgPath
    Push-Location $vcpkgPath
    .\bootstrap-vcpkg.bat -disableMetrics
    .\vcpkg integrate install
    Pop-Location
}
else {
    Write-ColorOutput $Green "vcpkg already installed."
    # Update vcpkg
    Push-Location $vcpkgPath
    git pull
    .\bootstrap-vcpkg.bat -disableMetrics
    Pop-Location
}

# Add vcpkg to path if not already there
if (-not ($env:Path -like "*$vcpkgPath*")) {
    $env:Path += ";$vcpkgPath"
    [Environment]::SetEnvironmentVariable("Path", $env:Path, [System.EnvironmentVariableTarget]::User)
}

# Install dependencies via vcpkg
Write-ColorOutput $Yellow "Installing OpenCog dependencies via vcpkg..."

$dependencies = @(
    "boost-filesystem:x64-windows",
    "boost-program-options:x64-windows",
    "boost-system:x64-windows",
    "boost-thread:x64-windows",
    "boost-math:x64-windows",
    "boost-uuid:x64-windows",
    "boost-date-time:x64-windows",
    "boost-random:x64-windows",
    "boost-serialization:x64-windows",
    "cxxtest:x64-windows",
    "rocksdb:x64-windows",
    "libpq:x64-windows",
    "sqlite3:x64-windows",
    "zlib:x64-windows",
    "doxygen:x64-windows"
)

foreach ($dep in $dependencies) {
    Write-ColorOutput $Yellow "Installing $dep..."
    & $vcpkgPath\vcpkg install $dep
    if ($LASTEXITCODE -ne 0) {
        Write-ColorOutput $Red "Failed to install $dep"
    }
}

# Set VCPKG_ROOT environment variable
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", $vcpkgPath, [System.EnvironmentVariableTarget]::User)
Write-ColorOutput $Green "VCPKG_ROOT environment variable set to: $vcpkgPath"

# Install Python packages
if (Get-Command "pip" -ErrorAction SilentlyContinue) {
    Write-ColorOutput $Yellow "Installing Python packages..."
    pip install numpy scipy cython pytest nose
}
else {
    Write-ColorOutput $Yellow "pip not found. Installing Python and pip..."
    $pythonInstaller = "$env:TEMP\python-installer.exe"
    Invoke-WebRequest -Uri "https://www.python.org/ftp/python/3.10.11/python-3.10.11-amd64.exe" -OutFile $pythonInstaller
    Start-Process -FilePath $pythonInstaller -Args "/quiet InstallAllUsers=1 PrependPath=1" -Wait
    # Refresh environment variables
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
    
    # Install Python packages
    pip install numpy scipy cython pytest nose
}

# Instructions for additional dependencies that can't be easily automated
Write-ColorOutput $Green "Dependencies installation completed!"
Write-ColorOutput $Yellow "NOTE: Some dependencies may need manual installation:"
Write-ColorOutput $Yellow "1. Guile Scheme: Download and install from http://www.gnu.org/software/guile/"
Write-ColorOutput $Yellow "2. Haskell (optional): Install using GHCup from https://www.haskell.org/ghcup/"
Write-ColorOutput $Yellow "3. OCaml (optional): Install using OPAM from https://ocaml.org/docs/installing-ocaml"

Write-Output ""
Write-ColorOutput $Green "You can now build OpenCog using the build.ps1 script."
Write-Output ""
Write-ColorOutput $Yellow "When configuring CMake projects, use:"
Write-Output "cmake .. -DCMAKE_TOOLCHAIN_FILE=$vcpkgPath\scripts\buildsystems\vcpkg.cmake" 