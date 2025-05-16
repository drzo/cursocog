#!/bin/bash
# 
# OpenCog Dependency Verification Tool - Bash Wrapper
# This script runs the verify-dependencies.py Python script
#

set -e  # Exit on error

# Color definitions
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR="$( cd "$SCRIPT_DIR/.." && pwd )"
PYTHON_SCRIPT="$SCRIPT_DIR/verify-dependencies.py"

# Check if Python is installed
PYTHON_CMD=""
for cmd in python3 python; do
    if command -v $cmd >/dev/null 2>&1; then
        PYTHON_CMD=$cmd
        break
    fi
done

if [ -z "$PYTHON_CMD" ]; then
    echo -e "${RED}Python is not installed or not in PATH. Please install Python 3.6 or newer.${NC}"
    echo -e "${YELLOW}You can install Python with:${NC}"
    echo "  sudo apt install python3 python3-pip   # for Debian/Ubuntu"
    echo "  brew install python                   # for macOS"
    exit 1
fi

# Make sure the script is executable
chmod +x "$PYTHON_SCRIPT"

# Run the verification script
echo -e "${BLUE}Running OpenCog Dependency Verification Tool...${NC}"
"$PYTHON_CMD" "$PYTHON_SCRIPT" "$@"
EXIT_CODE=$?

# If there were issues, suggest running the installer
if [ $EXIT_CODE -ne 0 ]; then
    echo
    echo -e "${YELLOW}To automatically install missing dependencies, run:${NC}"
    
    if [ "$(uname)" == "Darwin" ]; then
        # macOS
        echo "  brew install cmake boost cxxtest guile python3 rocksdb postgresql"
        echo "  pip3 install cython pytest numpy scipy"
    else
        # Linux
        echo "  sudo ./install-dependencies.sh"
    fi
fi

exit $EXIT_CODE 