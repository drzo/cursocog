#!/bin/bash
#
# OpenCog Build Script
# This script builds all OpenCog components in the required order
#

set -e  # Exit on error

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Create build log directory
LOGS_DIR="build_logs"
mkdir -p $LOGS_DIR

# Function to display error and exit
function error_exit {
    echo -e "${RED}$1${NC}" 1>&2
    exit 1
}

# Function to display section header
function section_header {
    echo -e "\n${BLUE}=== $1 ===${NC}"
}

# Get current directory
CURRENT_DIR=$(pwd)
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo -e "${BLUE}OpenCog Build System${NC}"
echo -e "${BLUE}===================${NC}"

# Check for dependencies
section_header "Checking dependencies"
echo "Verifying that all required dependencies are installed..."

# Run the dependency verification tool
if [ -f "$SCRIPT_DIR/scripts/verify-dependencies.sh" ]; then
    "$SCRIPT_DIR/scripts/verify-dependencies.sh"
    if [ $? -ne 0 ]; then
        read -p "Continue build despite dependency issues? (y/N): " confirm
        if [[ $confirm != [yY] ]]; then
            error_exit "Build aborted due to dependency issues."
        fi
        echo -e "${YELLOW}Continuing despite dependency issues. The build may fail.${NC}"
    else
        echo -e "${GREEN}All dependencies verified successfully.${NC}"
    fi
else
    echo -e "${YELLOW}Dependency verification tool not found. Skipping dependency check.${NC}"
    echo -e "${YELLOW}You may encounter build failures if dependencies are missing.${NC}"
    echo "Run ./install-dependencies.sh to install required dependencies."
fi

# Function to check if a command exists
function command_exists {
    command -v "$1" >/dev/null 2>&1
}

# Function to build a component
function build_component {
    local component_name=$1
    local component_dir=$2
    local log_file="$LOGS_DIR/${component_name}_build.log"
    
    echo -e "${YELLOW}Building $component_name...${NC}"
    
    # Check if component directory exists
    if [ ! -d "$component_dir" ]; then
        echo -e "${RED}Directory $component_dir not found. Cloning from GitHub...${NC}"
        git clone https://github.com/opencog/$component_name $component_dir
    fi
    
    # Create build directory
    mkdir -p $component_dir/build
    
    # Configure, build and install
    cd $component_dir/build
    echo "Configuring $component_name..."
    cmake .. > $log_file 2>&1 || error_exit "Failed to configure $component_name. See $log_file for details."
    
    echo "Building $component_name..."
    make -j$(nproc) >> $log_file 2>&1 || error_exit "Failed to build $component_name. See $log_file for details."
    
    echo "Installing $component_name..."
    make install >> $log_file 2>&1 || error_exit "Failed to install $component_name. See $log_file for details."
    
    # Run ldconfig to update shared library cache
    ldconfig >> $log_file 2>&1 || error_exit "Failed to run ldconfig after installing $component_name."
    
    cd ../../
    echo -e "${GREEN}Successfully built and installed $component_name.${NC}"
}

# Function to check prerequisites
function check_prerequisites {
    echo "Checking prerequisites..."
    
    # Check for essential build tools
    for cmd in git cmake make g++; do
        if ! command_exists $cmd; then
            error_exit "Required tool $cmd not found. Please install it and try again."
        fi
    done
    
    # Check for boost
    if ! ldconfig -p | grep -q libboost; then
        echo -e "${YELLOW}Warning: Boost libraries not found in ldconfig cache. Make sure they are installed.${NC}"
    fi
    
    # Check for other dependencies
    dependencies=(
        "libboost-dev"
        "libboost-filesystem-dev"
        "libboost-program-options-dev"
        "libboost-system-dev"
        "libboost-thread-dev"
        "cmake"
        "cxxtest"
        "binutils-dev"
        "libiberty-dev"
        "doxygen"
        "guile-3.0-dev"
        "libpq-dev"      # For PostgreSQL
        "librocksdb-dev" # For RocksDB
    )
    
    missing_deps=()
    for dep in "${dependencies[@]}"; do
        if ! dpkg -s $dep >/dev/null 2>&1; then
            missing_deps+=("$dep")
        fi
    done
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        echo -e "${YELLOW}The following dependencies are missing:${NC}"
        for dep in "${missing_deps[@]}"; do
            echo "  - $dep"
        done
        echo -e "${YELLOW}Consider installing them with:${NC}"
        echo "sudo apt-get install ${missing_deps[@]}"
    fi
    
    echo -e "${GREEN}Prerequisites check completed.${NC}"
}

# Main build process
function build_all {
    # Check prerequisites
    check_prerequisites
    
    # Build components in order
    build_component "cogutil" "cogutil"
    build_component "atomspace" "atomspace"
    build_component "unify" "unify"
    build_component "ure" "ure"
    build_component "atomspace-storage" "atomspace-storage"
    build_component "atomspace-rocks" "atomspace-rocks"
    build_component "atomspace-pgres" "atomspace-pgres"
    build_component "cogserver" "cogserver"
    build_component "attention" "attention"
    build_component "spacetime" "spacetime"
    build_component "pln" "pln"
    build_component "lg-atomese" "lg-atomese"
    build_component "learn" "learn"
    build_component "opencog" "opencog"
    build_component "sensory" "sensory"
    build_component "evidence" "evidence"
    
    echo -e "${GREEN}All components have been built and installed successfully!${NC}"
}

# Parse command line arguments
case "$1" in
    --help|-h)
        echo "Usage: $0 [OPTION]"
        echo "Build OpenCog components."
        echo ""
        echo "Options:"
        echo "  --check-only    Check prerequisites only without building"
        echo "  --component     Build a specific component"
        echo "  --help, -h      Display this help and exit"
        echo ""
        exit 0
        ;;
    --check-only)
        check_prerequisites
        exit 0
        ;;
    --component)
        if [ -z "$2" ]; then
            error_exit "Please specify a component name."
        fi
        build_component "$2" "$2"
        exit 0
        ;;
    *)
        build_all
        ;;
esac

exit 0 