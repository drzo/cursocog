#!/bin/bash
#
# OpenCog Dashboard Launcher
# This script launches the Clay UI dashboard for OpenCog Basic
#

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Launching OpenCog Dashboard...${NC}"

# Check if dashboard exists
if [ -f "./clay-ui/build/dashboard" ]; then
    # Launch dashboard
    ./clay-ui/build/dashboard $@
else
    echo -e "${RED}Dashboard executable not found.${NC}"
    echo -e "${YELLOW}Building dashboard...${NC}"
    
    # Try to build it
    if [ -f "./integrate-clay-ui.sh" ]; then
        echo -e "${YELLOW}Running Clay UI integration...${NC}"
        chmod +x ./integrate-clay-ui.sh
        ./integrate-clay-ui.sh
        
        # Check if build succeeded
        if [ -f "./clay-ui/build/dashboard" ]; then
            echo -e "${GREEN}Dashboard built successfully.${NC}"
            # Launch dashboard
            ./clay-ui/build/dashboard $@
        else
            echo -e "${RED}Failed to build dashboard.${NC}"
            echo -e "${YELLOW}Please run './integrate-clay-ui.sh' manually.${NC}"
            exit 1
        fi
    else
        echo -e "${RED}Integration script not found.${NC}"
        echo -e "${YELLOW}Please ensure you are in the opencog-basic directory.${NC}"
        exit 1
    fi
fi 