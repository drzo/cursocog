# INTEGRATE PLAN9 NAMESPACES with ATOMSPACE for WSL2 P9FS DEPLOYMENT

# PowerShell script to set up OpenCog in WSL
# This script automates the setup of a WSL environment for OpenCog development

Write-Host "OpenCog WSL Setup Script" -ForegroundColor Cyan
Write-Host "=======================" -ForegroundColor Cyan

# Check if WSL is installed
$wslCheck = wsl --status 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "WSL is not installed. Installing WSL..." -ForegroundColor Yellow
    wsl --install
    
    Write-Host "WSL installation initiated. Please restart your computer and run this script again." -ForegroundColor Green
    exit
}

# Check if Ubuntu is installed
$ubuntuCheck = wsl -l -v | Select-String "Ubuntu"
if (-not $ubuntuCheck) {
    Write-Host "Ubuntu distribution not found. Installing Ubuntu..." -ForegroundColor Yellow
    wsl --install -d Ubuntu
    
    Write-Host "Ubuntu installation initiated. Please wait for it to complete, set up your user account, and run this script again." -ForegroundColor Green
    exit
}

# Check if current directory is mounted in WSL
$currentDir = (Get-Location).Path
$wslPath = $currentDir -replace "\\", "/" -replace ":", ""
$wslPath = "/mnt/$wslPath"

Write-Host "Current directory: $currentDir" -ForegroundColor Gray
Write-Host "WSL path: $wslPath" -ForegroundColor Gray

# Set up OpenCog in WSL
Write-Host "Setting up OpenCog in WSL..." -ForegroundColor Yellow

# Install dependencies and set up OpenCog in WSL
wsl -d Ubuntu bash -c "
    echo 'Updating package lists...'
    sudo apt update

    echo 'Installing build dependencies...'
    sudo apt install -y build-essential cmake git

    echo 'Installing OpenCog dependencies...'
    sudo apt install -y libboost-all-dev python3-dev cxxtest

    echo 'Navigating to project directory...'
    mkdir -p $wslPath
    cd $wslPath

    # Create a simple build script
    cat > build-opencog.sh << 'EOL'
#!/bin/bash
# Build script for OpenCog components

set -e  # Exit on error

# List of components to build in order
COMPONENTS=(
    'cogutil'
    'atomspace'
    'atomspace-storage'
    'atomspace-rocks'
    'atomspace-pgres'
    'cogserver'
    'opencog'
)

# Function to build a component
build_component() {
    component=$1
    echo -e \"\n\n===================================\"
    echo \"Building $component...\"
    echo -e \"===================================\n\"
    
    if [ -d \"$component\" ]; then
        cd $component
        mkdir -p build
        cd build
        cmake ..
        make -j4
        sudo make install
        sudo ldconfig
        cd ../..
        echo \"$component built and installed successfully\"
    else
        echo \"$component directory not found, skipping\"
    fi
}

# Build each component in sequence
for component in \${COMPONENTS[@]}; do
    build_component $component
done

echo -e \"\n\nOpenCog build completed successfully!\n\"
EOL

    chmod +x build-opencog.sh
    echo 'Setup complete. You can now build OpenCog using the build-opencog.sh script in WSL.'
"

Write-Host "`nSetup complete!" -ForegroundColor Green
Write-Host "To build OpenCog in WSL:" -ForegroundColor Cyan
Write-Host "1. Open a WSL terminal: wsl" -ForegroundColor White
Write-Host "2. Navigate to your project: cd $wslPath" -ForegroundColor White
Write-Host "3. Run the build script: ./build-opencog.sh" -ForegroundColor White
Write-Host "`nThis will build the components in the following order:" -ForegroundColor Cyan
Write-Host "cogutil → atomspace → atomspace-storage → atomspace-rocks → atomspace-pgres → cogserver → opencog" -ForegroundColor White 