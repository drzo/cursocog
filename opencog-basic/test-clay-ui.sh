#!/bin/bash
#
# OpenCog Basic - Clay UI Test Script
# This script verifies the Clay UI integration and functionality
#

set -e  # Exit on error

echo "OpenCog Basic - Clay UI Test and Validation"
echo "==========================================="
echo

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to check for required dependencies
check_dependencies() {
    echo -e "${BLUE}Checking for required dependencies...${NC}"
    
    local missing_deps=0
    local deps=("cmake" "g++" "make" "pkg-config" "libgl1-mesa-dev" "libglu1-mesa-dev" "libglew-dev")
    
    for dep in "${deps[@]}"; do
        if ! command -v $dep &> /dev/null && ! dpkg -l | grep -q $dep; then
            echo -e "${RED}Missing dependency: $dep${NC}"
            missing_deps=1
        fi
    done
    
    if [ $missing_deps -eq 1 ]; then
        echo -e "${RED}Please install missing dependencies with:${NC}"
        echo -e "${YELLOW}sudo apt-get install cmake g++ make pkg-config libgl1-mesa-dev libglu1-mesa-dev libglew-dev${NC}"
        return 1
    else
        echo -e "${GREEN}All dependencies are installed.${NC}"
        return 0
    fi
}

# Function to check directory structure
check_directory_structure() {
    echo -e "${BLUE}Checking Clay UI directory structure...${NC}"
    
    local missing=0
    local directories=("clay-ui" "clay-ui/include" "clay-ui/src" "clay-ui/examples" "clay-ui/cmake")
    
    for dir in "${directories[@]}"; do
        if [ ! -d "$dir" ]; then
            echo -e "${RED}Missing directory: $dir${NC}"
            missing=1
        fi
    done
    
    local files=("clay-ui/include/clay.h" "clay-ui/include/opencog_renderer.h" "clay-ui/include/opencog_atomspace_bridge.h" "clay-ui/CMakeLists.txt")
    
    for file in "${files[@]}"; do
        if [ ! -f "$file" ]; then
            echo -e "${RED}Missing file: $file${NC}"
            missing=1
        fi
    done
    
    if [ $missing -eq 1 ]; then
        echo -e "${RED}Clay UI directory structure is incomplete. Please run integrate-clay-ui.sh first.${NC}"
        return 1
    else
        echo -e "${GREEN}Clay UI directory structure is complete.${NC}"
        return 0
    fi
}

# Function to build the dashboard
build_dashboard() {
    echo -e "${BLUE}Building dashboard...${NC}"
    
    # Create build directory if it doesn't exist
    if [ ! -d "clay-ui/build" ]; then
        mkdir -p clay-ui/build
    fi
    
    # Build
    pushd clay-ui/build > /dev/null
    
    if ! cmake .. -DCMAKE_BUILD_TYPE=Release; then
        echo -e "${RED}CMake configuration failed.${NC}"
        popd > /dev/null
        return 1
    fi
    
    if ! cmake --build . --target dashboard --config Release; then
        echo -e "${RED}Build failed.${NC}"
        popd > /dev/null
        return 1
    fi
    
    popd > /dev/null
    
    # Check if executable was created
    if [ ! -f "clay-ui/build/dashboard" ]; then
        echo -e "${RED}Dashboard executable was not created.${NC}"
        return 1
    fi
    
    echo -e "${GREEN}Dashboard built successfully.${NC}"
    return 0
}

# Function to verify AtomSpace integration
verify_atomspace_integration() {
    echo -e "${BLUE}Verifying AtomSpace integration...${NC}"
    
    if ! grep -q "OpenCogAtomSpaceBridge_Initialize" clay-ui/include/opencog_atomspace_bridge.h; then
        echo -e "${RED}AtomSpace bridge integration is missing.${NC}"
        return 1
    fi
    
    echo -e "${GREEN}AtomSpace integration verified.${NC}"
    return 0
}

# Function to test launcher script
test_launcher_script() {
    echo -e "${BLUE}Testing dashboard launcher script...${NC}"
    
    if [ ! -f "opencog-dashboard.sh" ]; then
        echo -e "${RED}Launcher script 'opencog-dashboard.sh' is missing.${NC}"
        return 1
    fi
    
    if [ ! -x "opencog-dashboard.sh" ]; then
        echo -e "${YELLOW}Setting executable permissions on launcher script...${NC}"
        chmod +x "opencog-dashboard.sh"
    fi
    
    echo -e "${GREEN}Launcher script verified.${NC}"
    return 0
}

# Function to generate test report
generate_report() {
    local dep_status=$1
    local dir_status=$2
    local build_status=$3
    local integration_status=$4
    local launcher_status=$5
    
    echo
    echo -e "${BLUE}====== Clay UI Test Report ======${NC}"
    echo
    echo -e "Dependencies check: $([ $dep_status -eq 0 ] && echo -e "${GREEN}PASS${NC}" || echo -e "${RED}FAIL${NC}")"
    echo -e "Directory structure: $([ $dir_status -eq 0 ] && echo -e "${GREEN}PASS${NC}" || echo -e "${RED}FAIL${NC}")"
    echo -e "Build test: $([ $build_status -eq 0 ] && echo -e "${GREEN}PASS${NC}" || echo -e "${RED}FAIL${NC}")"
    echo -e "AtomSpace integration: $([ $integration_status -eq 0 ] && echo -e "${GREEN}PASS${NC}" || echo -e "${RED}FAIL${NC}")"
    echo -e "Launcher script: $([ $launcher_status -eq 0 ] && echo -e "${GREEN}PASS${NC}" || echo -e "${RED}FAIL${NC}")"
    echo
    
    # Calculate overall status
    local total_status=$(( $dep_status + $dir_status + $build_status + $integration_status + $launcher_status ))
    
    if [ $total_status -eq 0 ]; then
        echo -e "${GREEN}All tests passed! Clay UI is properly integrated.${NC}"
        echo -e "${YELLOW}You can run the dashboard with: ./opencog-dashboard.sh${NC}"
    else
        echo -e "${RED}Some tests failed. Please address the issues above.${NC}"
    fi
}

# Main execution flow
check_dependencies
dep_status=$?

check_directory_structure
dir_status=$?

build_dashboard
build_status=$?

verify_atomspace_integration
integration_status=$?

test_launcher_script
launcher_status=$?

# Generate test report
generate_report $dep_status $dir_status $build_status $integration_status $launcher_status

exit $(( $dep_status + $dir_status + $build_status + $integration_status + $launcher_status )) 