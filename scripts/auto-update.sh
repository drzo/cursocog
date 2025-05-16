#!/bin/bash
#
# OpenCog Auto-Update Bash Wrapper
# This script provides a Unix-friendly wrapper for the auto-update.py script
#

set -e  # Exit on error

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR="$( cd "$SCRIPT_DIR/.." && pwd )"

# Python script path
PYTHON_SCRIPT="$SCRIPT_DIR/auto-update.py"

# Default action
ACTION="check"

# Function to display error and exit
function error_exit {
    echo -e "${RED}$1${NC}" 1>&2
    exit 1
}

# Function to display help
function show_help {
    echo -e "${BLUE}OpenCog Auto-Update System${NC}"
    echo ""
    echo "Usage:"
    echo "  $(basename $0) [action] [options]"
    echo ""
    echo "Actions:"
    echo "  check    Check for available updates (default)"
    echo "  update   Download and install available updates"
    echo ""
    echo "Options:"
    echo "  --silent      Don't display progress information"
    echo "  --log=FILE    Log output to FILE instead of stdout"
    echo "  --help        Show this help message"
    echo "  --version     Show version information"
    exit 0
}

# Function to display version
function show_version {
    echo "OpenCog Auto-Update System v1.0.0"
    exit 0
}

# Check if Python is available
function check_python {
    if ! command -v python3 &> /dev/null; then
        error_exit "Python 3 is not installed or not in the PATH. Please install Python 3.6 or later."
    fi
}

# Parse command line arguments
SILENT=""
LOG_FILE=""

while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        check|update)
            ACTION="$1"
            shift
            ;;
        --silent)
            SILENT="--silent"
            shift
            ;;
        --log=*)
            LOG_FILE="--log=${key#*=}"
            shift
            ;;
        --log)
            LOG_FILE="--log=$2"
            shift 2
            ;;
        --help)
            show_help
            ;;
        --version)
            show_version
            ;;
        *)
            error_exit "Unknown option: $1"
            ;;
    esac
done

# Check for Python
check_python

# Run the Python script
echo -e "${BLUE}Running auto-update.py $ACTION...${NC}"

PYTHON_CMD="python3"
if ! command -v python3 &> /dev/null; then
    # Fall back to python if python3 is not available
    PYTHON_CMD="python"
fi

$PYTHON_CMD "$PYTHON_SCRIPT" "$ACTION" $SILENT $LOG_FILE

exit $? 