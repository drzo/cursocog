#!/bin/bash
#
# OpenCog Component Configuration System
# This script manages component-specific configuration options
#

set -e  # Exit on error

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration directory
CONFIG_DIR=".opencog-config"
mkdir -p $CONFIG_DIR

# Function to display error and exit
function error_exit {
    echo -e "${RED}$1${NC}" 1>&2
    exit 1
}

# Function to display section header
function section_header {
    echo -e "\n${BLUE}=== $1 ===${NC}"
}

# Function to get configuration for a component
function get_component_config {
    local component=$1
    local config_file="$CONFIG_DIR/${component}.conf"
    
    if [ -f "$config_file" ]; then
        cat "$config_file"
    else
        echo "# Default configuration for $component"
        echo "# Uncomment and modify options as needed"
        
        # Default options for each component
        case "$component" in
            "cogutil")
                echo "#BUILD_SHARED_LIBS=ON"
                echo "#BUILD_TESTS=ON"
                ;;
            "atomspace")
                echo "#BUILD_SHARED_LIBS=ON"
                echo "#BUILD_TESTS=ON"
                echo "#CXXTEST_BIN_DIR=/usr/local/bin"
                ;;
            "atomspace-rocks")
                echo "#BUILD_SHARED_LIBS=ON"
                echo "#ROCKSDB_INCLUDE_DIR=/usr/local/include"
                echo "#ROCKSDB_LIBRARY_DIR=/usr/local/lib"
                ;;
            "atomspace-pgres")
                echo "#BUILD_SHARED_LIBS=ON"
                echo "#PGSQL_INCLUDE_DIR=/usr/include/postgresql"
                echo "#PGSQL_LIBRARY_DIR=/usr/lib"
                ;;
            "cogserver")
                echo "#BUILD_SHARED_LIBS=ON"
                echo "#ENABLE_NETWORK_SERVER=ON"
                ;;
            "learn")
                echo "#BUILD_SHARED_LIBS=ON"
                echo "#LINK_GRAMMAR_INCLUDE_DIR=/usr/local/include"
                echo "#LINK_GRAMMAR_LIBRARY_DIR=/usr/local/lib"
                ;;
            "opencog")
                echo "#BUILD_SHARED_LIBS=ON"
                echo "#ENABLE_LEARNING=ON"
                echo "#ENABLE_PLN=ON"
                echo "#ENABLE_ATTENTION=ON"
                ;;
            "sensory")
                echo "#BUILD_SHARED_LIBS=ON"
                echo "#ENABLE_VISION=ON"
                echo "#ENABLE_AUDIO=ON"
                ;;
            *)
                echo "#BUILD_SHARED_LIBS=ON"
                echo "#BUILD_TESTS=ON"
                ;;
        esac
    fi
}

# Function to set configuration for a component
function set_component_config {
    local component=$1
    local option=$2
    local value=$3
    local config_file="$CONFIG_DIR/${component}.conf"
    
    # Create config file if it doesn't exist
    if [ ! -f "$config_file" ]; then
        get_component_config "$component" > "$config_file"
    fi
    
    # Check if option already exists
    if grep -q "^#\?${option}=" "$config_file"; then
        # Update existing option
        sed -i "s|^#\?${option}=.*|${option}=${value}|" "$config_file"
    else
        # Add new option
        echo "${option}=${value}" >> "$config_file"
    fi
    
    echo -e "${GREEN}Set ${option}=${value} for ${component}${NC}"
}

# Function to generate CMake arguments from config
function generate_cmake_args {
    local component=$1
    local config_file="$CONFIG_DIR/${component}.conf"
    local cmake_args=""
    
    if [ -f "$config_file" ]; then
        while IFS='=' read -r key value; do
            # Skip comments and empty lines
            if [[ ! "$key" =~ ^#.*$ && -n "$key" ]]; then
                cmake_args="$cmake_args -D$key=$value"
            fi
        done < "$config_file"
    fi
    
    echo "$cmake_args"
}

# Function to list available components
function list_components {
    echo -e "${BLUE}Available components:${NC}"
    echo "cogutil"
    echo "atomspace"
    echo "unify"
    echo "ure"
    echo "atomspace-storage"
    echo "atomspace-rocks"
    echo "atomspace-pgres"
    echo "cogserver"
    echo "attention"
    echo "spacetime"
    echo "pln"
    echo "lg-atomese"
    echo "learn"
    echo "opencog"
    echo "sensory"
    echo "evidence"
}

# Function to display configuration for a component
function show_component_config {
    local component=$1
    local config_file="$CONFIG_DIR/${component}.conf"
    
    echo -e "${BLUE}Configuration for ${component}:${NC}"
    
    if [ -f "$config_file" ]; then
        cat "$config_file"
    else
        echo "No custom configuration set. Using defaults."
        get_component_config "$component"
    fi
}

# Parse command line arguments
case "$1" in
    get)
        if [ -z "$2" ]; then
            error_exit "Please specify a component name."
        fi
        get_component_config "$2"
        ;;
    set)
        if [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ]; then
            error_exit "Usage: $0 set <component> <option> <value>"
        fi
        set_component_config "$2" "$3" "$4"
        ;;
    show)
        if [ -z "$2" ]; then
            error_exit "Please specify a component name."
        fi
        show_component_config "$2"
        ;;
    args)
        if [ -z "$2" ]; then
            error_exit "Please specify a component name."
        fi
        generate_cmake_args "$2"
        ;;
    list)
        list_components
        ;;
    --help|-h)
        echo "Usage: $0 <command> [options]"
        echo "Manage component-specific configuration options."
        echo ""
        echo "Commands:"
        echo "  get <component>                  Get configuration template for a component"
        echo "  set <component> <option> <value> Set configuration option for a component"
        echo "  show <component>                 Show current configuration for a component"
        echo "  args <component>                 Generate CMake arguments for a component"
        echo "  list                             List available components"
        echo "  --help, -h                       Display this help and exit"
        echo ""
        exit 0
        ;;
    *)
        error_exit "Unknown command. Use --help for usage information."
        ;;
esac

exit 0 