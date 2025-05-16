#!/bin/bash
#
# OpenCog Basic - CogInit Integration
# This script integrates the CogInit dependency manager into the OpenCog Basic repository
#

set -e  # Exit on error

echo "OpenCog Basic - CogInit Integration"
echo "=================================="
echo

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to display error and exit
function error_exit {
    echo -e "${RED}$1${NC}" 1>&2
    exit 1
}

# Check if coginit exists in parent directory
if [ ! -d "../coginit" ]; then
    error_exit "CogInit directory not found in parent directory. Please run coginit.sh first."
fi

# Create coginit subdirectory in opencog-basic
COGINIT_DIR="coginit"
mkdir -p $COGINIT_DIR

# Copy coginit files to opencog-basic
echo -e "${YELLOW}Copying CogInit files to OpenCog Basic...${NC}"

# Create subdirectories
for dir in deps scripts tests docs; do
    mkdir -p "$COGINIT_DIR/$dir"
done

# Copy essential files
cp -r ../coginit/dependencies.json $COGINIT_DIR/
cp -r ../coginit/deps/* $COGINIT_DIR/deps/ 2>/dev/null || true
cp -r ../coginit/scripts/* $COGINIT_DIR/scripts/ 2>/dev/null || true
cp -r ../coginit/docs/* $COGINIT_DIR/docs/ 2>/dev/null || true
cp -r ../coginit/.gitignore $COGINIT_DIR/ 2>/dev/null || true

# Make scripts executable
chmod +x $COGINIT_DIR/scripts/*.sh 2>/dev/null || true

# Create a README file for coginit in opencog-basic
echo -e "${YELLOW}Creating CogInit README...${NC}"
cat > $COGINIT_DIR/README.md << 'EOF'
# CogInit - Dependency Manager for OpenCog Basic

This directory contains the CogInit dependency management system integrated with OpenCog Basic.
It provides tools for managing, installing, and verifying all dependencies required to build OpenCog.

## Usage

### Prerequisites Installation

```bash
# On Linux
./coginit/scripts/install-linux.sh

# On Windows
.\coginit\scripts\install-windows.ps1
```

### Verification

```bash
# On Linux
./coginit/scripts/verify-linux.sh

# On Windows
.\coginit\scripts\verify-windows.ps1
```

## More Information

For detailed instructions, please see the main README.md file in the root directory.
EOF

# Update the main build scripts to use coginit
echo -e "${YELLOW}Updating main build script to use CogInit...${NC}"

# Create wrapper build script that uses coginit
cat > build-with-coginit.sh << 'EOF'
#!/bin/bash
#
# OpenCog Basic - Build with CogInit
# This script builds OpenCog Basic components using CogInit for dependency management
#

set -e  # Exit on error

echo "OpenCog Basic - Build with CogInit"
echo "=================================="
echo

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Verify dependencies are installed
echo -e "${YELLOW}Verifying dependencies...${NC}"
if [ -f "./coginit/scripts/verify-linux.sh" ]; then
    ./coginit/scripts/verify-linux.sh || {
        echo -e "${RED}Some dependencies are missing or have issues.${NC}"
        echo -e "${YELLOW}Would you like to install them now? (y/n)${NC}"
        read -r response
        if [[ "$response" =~ ^([yY][eE][sS]|[yY])$ ]]; then
            echo -e "${YELLOW}Installing dependencies...${NC}"
            ./coginit/scripts/install-linux.sh
        else
            echo -e "${RED}Build aborted. Please install dependencies and try again.${NC}"
            exit 1
        fi
    }
else
    echo -e "${YELLOW}Verification script not found. Skipping dependency check.${NC}"
fi

# Run the original build script
echo -e "${YELLOW}Building OpenCog Basic components...${NC}"
if [ -f "./build.sh" ]; then
    ./build.sh
else
    echo -e "${RED}Build script not found. Cannot continue.${NC}"
    exit 1
fi

echo -e "${GREEN}Build with CogInit completed successfully!${NC}"
EOF

# Make the script executable
chmod +x build-with-coginit.sh

# Create Windows version of the wrapper build script
cat > build-with-coginit.ps1 << 'EOF'
# OpenCog Basic - Build with CogInit
# This script builds OpenCog Basic components using CogInit for dependency management

Write-Host "OpenCog Basic - Build with CogInit" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
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

# Verify dependencies are installed
Write-ColorOutput $Yellow "Verifying dependencies..."
if (Test-Path ".\coginit\scripts\verify-windows.ps1") {
    & .\coginit\scripts\verify-windows.ps1
    if ($LASTEXITCODE -ne 0) {
        Write-ColorOutput $Red "Some dependencies are missing or have issues."
        $response = Read-Host "Would you like to install them now? (y/n)"
        if ($response -eq "y" -or $response -eq "Y") {
            Write-ColorOutput $Yellow "Installing dependencies..."
            & .\coginit\scripts\install-windows.ps1
        }
        else {
            Write-ColorOutput $Red "Build aborted. Please install dependencies and try again."
            exit 1
        }
    }
}
else {
    Write-ColorOutput $Yellow "Verification script not found. Skipping dependency check."
}

# Run the original build script
Write-ColorOutput $Yellow "Building OpenCog Basic components..."
if (Test-Path ".\build.ps1") {
    & .\build.ps1
}
else {
    Write-ColorOutput $Red "Build script not found. Cannot continue."
    exit 1
}

Write-ColorOutput $Green "Build with CogInit completed successfully!"
EOF

# Update main README.md to include information about coginit
echo -e "${YELLOW}Updating main README.md to include CogInit information...${NC}"

# Check if README.md exists
if [ -f "README.md" ]; then
    # Add section about coginit
    cat >> README.md << 'EOF'

## Dependency Management with CogInit

This repository includes CogInit, a dependency management system for OpenCog. CogInit simplifies the setup process by providing:

1. Local copies of dependency source files
2. Automated installation scripts
3. Verification tools

### Using CogInit

To build with automatic dependency management:

```bash
# On Linux
./build-with-coginit.sh

# On Windows
.\build-with-coginit.ps1
```

These scripts will verify dependencies are installed before building and offer to install missing ones.

### Manual Dependency Management

You can also manage dependencies directly:

```bash
# Install dependencies on Linux
./coginit/scripts/install-linux.sh

# Install dependencies on Windows
.\coginit\scripts\install-windows.ps1
```

For more information, see the README in the [coginit](./coginit) directory.
EOF
    echo -e "${GREEN}README.md updated successfully.${NC}"
else
    echo -e "${RED}README.md not found. Skipping update.${NC}"
fi

echo
echo -e "${GREEN}CogInit integration complete!${NC}"
echo "You can now use the CogInit dependency management system with OpenCog Basic."
echo
echo -e "${YELLOW}Recommended next steps:${NC}"
echo "1. Review the updated README.md"
echo "2. Run ./build-with-coginit.sh to build with dependency management"
echo "3. Verify all dependencies with ./coginit/scripts/verify-linux.sh" 