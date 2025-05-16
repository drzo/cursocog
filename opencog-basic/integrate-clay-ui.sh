#!/bin/bash
#
# OpenCog Basic - Clay UI Integration Wrapper
# This script integrates and builds the Clay UI dashboard
#

set -e  # Exit on error

echo "OpenCog Basic - Clay UI Integration and Build"
echo "==========================================="
echo

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Run the integration script
if [ -f "./clay-ui-integration.sh" ]; then
    echo -e "${YELLOW}Running Clay UI integration script...${NC}"
    chmod +x ./clay-ui-integration.sh
    ./clay-ui-integration.sh
else
    echo -e "${RED}ERROR: clay-ui-integration.sh not found.${NC}"
    exit 1
fi

# Create build directory
if [ ! -d "./clay-ui/build" ]; then
    echo -e "${YELLOW}Creating Clay UI build directory...${NC}"
    mkdir -p ./clay-ui/build
fi

# Build the dashboard
echo -e "${YELLOW}Building OpenCog Dashboard...${NC}"
pushd ./clay-ui/build > /dev/null

# Run CMake
echo -e "${YELLOW}Running CMake...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo -e "${YELLOW}Compiling dashboard...${NC}"
cmake --build . --target dashboard --config Release

popd > /dev/null

# Check if build succeeded
if [ -f "./clay-ui/build/dashboard" ]; then
    echo -e "${GREEN}Dashboard built successfully!${NC}"
    echo
    echo -e "${YELLOW}You can now run the dashboard with: ./opencog-dashboard.sh${NC}"
else
    echo -e "${RED}Dashboard build failed. Please check the error messages above.${NC}"
fi 