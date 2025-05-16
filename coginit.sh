#!/bin/bash
#
# OpenCog CogInit - Dependencies and Prerequisites Manager
# This script creates a new repository with optimized local copies of all OpenCog prerequisites
#

set -e  # Exit on error

echo "OpenCog CogInit - Dependencies Manager"
echo "======================================"
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

# Create directory for the coginit repository
REPO_DIR="coginit"
mkdir -p $REPO_DIR

# Create essential subdirectories
DEPS_DIR="$REPO_DIR/deps"
SCRIPTS_DIR="$REPO_DIR/scripts"
TESTS_DIR="$REPO_DIR/tests"
DOCS_DIR="$REPO_DIR/docs"

for dir in $DEPS_DIR $SCRIPTS_DIR $TESTS_DIR $DOCS_DIR; do
    mkdir -p $dir
done

# Create logs directory
LOGS_DIR="downloads_logs"
mkdir -p $LOGS_DIR

echo -e "${YELLOW}Setting up dependency information...${NC}"

# Create dependencies.json to store dependency information
cat > $REPO_DIR/dependencies.json << 'EOF'
[
  {
    "Name": "boost",
    "Version": "1.79.0",
    "Url": "https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.gz",
    "FileName": "boost_1_79_0.tar.gz",
    "Description": "C++ utilities package",
    "Essential": true
  },
  {
    "Name": "cmake",
    "Version": "3.25.1",
    "Url": "https://github.com/Kitware/CMake/releases/download/v3.25.1/cmake-3.25.1.tar.gz",
    "FileName": "cmake-3.25.1.tar.gz",
    "Description": "Build management tool",
    "Essential": true
  },
  {
    "Name": "cxxtest",
    "Version": "4.4",
    "Url": "https://github.com/CxxTest/cxxtest/releases/download/4.4/cxxtest-4.4.tar.gz",
    "FileName": "cxxtest-4.4.tar.gz",
    "Description": "Unit test framework",
    "Essential": true
  },
  {
    "Name": "guile",
    "Version": "3.0.9",
    "Url": "https://ftp.gnu.org/gnu/guile/guile-3.0.9.tar.gz",
    "FileName": "guile-3.0.9.tar.gz",
    "Description": "Embedded scheme REPL",
    "Essential": true
  },
  {
    "Name": "openssl",
    "Version": "3.0.8",
    "Url": "https://www.openssl.org/source/openssl-3.0.8.tar.gz",
    "FileName": "openssl-3.0.8.tar.gz",
    "Description": "SSL/TLS implementation",
    "Essential": false
  },
  {
    "Name": "binutils",
    "Version": "2.39",
    "Url": "https://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz",
    "FileName": "binutils-2.39.tar.gz",
    "Description": "GNU Binary File Description library",
    "Essential": false
  },
  {
    "Name": "doxygen",
    "Version": "1.9.6",
    "Url": "https://www.doxygen.nl/files/doxygen-1.9.6.src.tar.gz",
    "FileName": "doxygen-1.9.6.src.tar.gz",
    "Description": "Documentation generator",
    "Essential": false
  }
]
EOF

# Function to download a file if it doesn't exist
function download_file {
    local url=$1
    local output_path=$2
    local log_file=$3
    
    if [ ! -f "$output_path" ]; then
        echo -e "${YELLOW}Downloading $url...${NC}"
        if curl -L "$url" -o "$output_path" --progress-bar; then
            echo -e "${GREEN}Downloaded successfully to $output_path${NC}"
            return 0
        else
            echo -e "${RED}Failed to download $url${NC}"
            echo "Error downloading $url" >> "$log_file"
            return 1
        fi
    else
        echo -e "${YELLOW}File already exists: $output_path${NC}"
        return 0
    fi
}

# Check if jq is installed
if ! command -v jq &> /dev/null; then
    echo -e "${YELLOW}jq is required for processing JSON. Please install it with your package manager.${NC}"
    echo "For example: sudo apt install jq"
    exit 1
fi

# Download essential dependencies
echo -e "${YELLOW}Downloading dependency packages...${NC}"

# Read each dependency from the JSON file
jq -c '.[]' $REPO_DIR/dependencies.json | while read -r dep; do
    name=$(echo $dep | jq -r '.Name')
    version=$(echo $dep | jq -r '.Version')
    url=$(echo $dep | jq -r '.Url')
    filename=$(echo $dep | jq -r '.FileName')
    essential=$(echo $dep | jq -r '.Essential')
    
    echo "Processing $name v$version..."
    
    if [ "$essential" = "true" ]; then
        dest_file="$DEPS_DIR/$filename"
        log_file="$LOGS_DIR/${name}_download.log"
        
        if ! download_file "$url" "$dest_file" "$log_file"; then
            if [ "$essential" = "true" ]; then
                error_exit "Failed to download essential dependency: $name"
            fi
        fi
    else
        echo "  (Optional dependency - will download on demand)"
    fi
done

# Create README.md
echo -e "${YELLOW}Creating README.md...${NC}"
cat > $REPO_DIR/README.md << 'EOF'
# CogInit - OpenCog Prerequisites Manager

CogInit is a tool for managing all prerequisites and dependencies needed for building OpenCog components.
It includes local copies of source code, installation scripts, and verification tests.

## Contents

- **deps/** - Local source code copies of all dependencies
- **scripts/** - Installation scripts for different platforms
- **tests/** - Verification tests for each dependency
- **docs/** - Comprehensive documentation

## Getting Started

### On Linux/macOS:

```bash
# Install all dependencies
./scripts/install-linux.sh

# Verify installation
./scripts/verify-linux.sh
```

### On Windows:

```powershell
# Install all dependencies
.\scripts\install-windows.ps1

# Verify installation
.\scripts\verify-windows.ps1
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

Edit the `dependencies.json` file to add or modify dependencies.
EOF

# Create Linux installation script
echo -e "${YELLOW}Creating Linux installation script...${NC}"
cat > $SCRIPTS_DIR/install-linux.sh << 'EOF'
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
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
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
DEPENDENCIES=$(jq -c '.[]' "$DEPS_JSON")

# Process each dependency
echo -e "${YELLOW}Installing OpenCog prerequisites...${NC}"

echo "$DEPENDENCIES" | while read -r DEP; do
    NAME=$(echo $DEP | jq -r '.Name')
    VERSION=$(echo $DEP | jq -r '.Version')
    FILENAME=$(echo $DEP | jq -r '.FileName')
    ESSENTIAL=$(echo $DEP | jq -r '.Essential')
    DESCRIPTION=$(echo $DEP | jq -r '.Description')
    
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
EOF

# Create Linux verification script
echo -e "${YELLOW}Creating Linux verification script...${NC}"
cat > $SCRIPTS_DIR/verify-linux.sh << 'EOF'
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
        if ((10#${ver1[i]} > 10#${ver2[i]})); then
            return 1
        fi
        if ((10#${ver1[i]} < 10#${ver2[i]})); then
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
EOF

# Create documentation files
echo -e "${YELLOW}Creating Boost documentation...${NC}"
cat > $DOCS_DIR/boost_linux.md << 'EOF'
# Building Boost on Linux

This guide explains how to build Boost from source on Linux for OpenCog development.

## Prerequisites

- GCC 7.0 or later
- Python 3.6 or later
- About 2GB of free disk space

## Building Steps

1. Extract the Boost archive from `../deps/boost_1_79_0.tar.gz` to a directory on your system:
   ```
   tar -xzf ../deps/boost_1_79_0.tar.gz -C /path/to/extract
   ```

2. Navigate to the Boost directory:
   ```
   cd /path/to/extract/boost_1_79_0
   ```

3. Run bootstrap:
   ```
   ./bootstrap.sh --prefix=/usr/local
   ```

4. Build Boost with the required libraries:
   ```
   ./b2 --with-system --with-filesystem --with-thread --with-program_options --with-regex -j4 install
   ```

   Note: `-j4` specifies using 4 cores for compilation. Adjust according to your system.
   The `install` command will require sudo privileges.

## System-wide Installation

To install the built libraries system-wide:

```
sudo ./b2 --with-system --with-filesystem --with-thread --with-program_options --with-regex -j4 install
```

This will install Boost to `/usr/local/lib` and `/usr/local/include`.

## Verification

To verify your Boost installation:

1. Check if the libraries exist:
   ```
   ls -la /usr/local/lib/libboost*
   ```

2. Make sure the include files are available:
   ```
   ls -la /usr/local/include/boost
   ```

3. Update the dynamic linker run-time bindings:
   ```
   sudo ldconfig
   ```

4. Test if a simple Boost program compiles:
   ```cpp
   #include <boost/version.hpp>
   #include <iostream>
   
   int main() {
       std::cout << "Boost version: " 
                 << BOOST_VERSION / 100000 << "."  // major version
                 << BOOST_VERSION / 100 % 1000 << "."  // minor version
                 << BOOST_VERSION % 100  // patch level
                 << std::endl;
       return 0;
   }
   ```

   Compile with:
   ```
   g++ -o boost_test boost_test.cpp
   ```
EOF

# Create gitignore file
echo -e "${YELLOW}Creating .gitignore...${NC}"
cat > $REPO_DIR/.gitignore << 'EOF'
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
EOF

# Make scripts executable
chmod +x $SCRIPTS_DIR/install-linux.sh
chmod +x $SCRIPTS_DIR/verify-linux.sh

# Final message
echo
echo -e "${GREEN}CogInit repository setup complete!${NC}"
echo "The coginit directory contains all necessary files for managing OpenCog prerequisites."
echo "To use it:"
echo "1. Copy the coginit directory to your desired location"
echo "2. Run the appropriate installation script for your platform:"
echo "   - Linux: ./scripts/install-linux.sh"
echo "   - Windows: ./scripts/install-windows.ps1"
echo ""
echo "You can verify the installation with:"
echo "   - Linux: ./scripts/verify-linux.sh"
echo "   - Windows: ./scripts/verify-windows.ps1" 