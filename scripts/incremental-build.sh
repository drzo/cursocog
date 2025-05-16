#!/bin/bash
#
# OpenCog Incremental Build Script
# This script builds only the components that have changed since the last build
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

# Create state directory to track last build times
STATE_DIR=".opencog-build-state"
mkdir -p $STATE_DIR

# Function to display error and exit
function error_exit {
    echo -e "${RED}$1${NC}" 1>&2
    exit 1
}

# Function to display section header
function section_header {
    echo -e "\n${BLUE}=== $1 ===${NC}"
}

# Function to check if a component has changed since last build
function has_component_changed {
    local component_dir=$1
    local state_file="$STATE_DIR/${component_dir//\//_}.timestamp"
    
    # If state file doesn't exist, component hasn't been built yet
    if [ ! -f "$state_file" ]; then
        return 0  # True, component has changed
    fi
    
    # Get last build time
    local last_build_time=$(cat "$state_file")
    
    # Check if any file in the component directory has changed
    if find "$component_dir" -type f -newer "$state_file" -not -path "*/build/*" -not -path "*/\.*" | grep -q .; then
        return 0  # True, component has changed
    fi
    
    # Check if any dependency has been rebuilt since last build
    for dep in $(get_dependencies "$component_dir"); do
        local dep_state_file="$STATE_DIR/${dep//\//_}.timestamp"
        if [ -f "$dep_state_file" ] && [ "$dep_state_file" -nt "$state_file" ]; then
            return 0  # True, dependency has changed
        fi
    done
    
    return 1  # False, component has not changed
}

# Function to get the dependencies of a component
function get_dependencies {
    local component_dir=$1
    local dependencies=()
    
    # Parse CMakeLists.txt to find dependencies
    if [ -f "$component_dir/CMakeLists.txt" ]; then
        # Extract project name
        local project_name=$(grep -m 1 "project" "$component_dir/CMakeLists.txt" | sed -E 's/.*project\s*\(\s*([^ )]+).*/\1/')
        
        # Find components that this one depends on
        if [ "$project_name" == "atomspace" ]; then
            # AtomSpace depends only on cogutil
            dependencies+=("cogutil")
        elif [ "$project_name" == "atomspace-rocks" ] || [ "$project_name" == "atomspace-pgres" ]; then
            # AtomSpace storage backends depend on atomspace and atomspace-storage
            dependencies+=("cogutil" "atomspace" "atomspace-storage")
        elif [ "$project_name" == "cogserver" ]; then
            # CogServer depends on atomspace
            dependencies+=("cogutil" "atomspace")
        elif [ "$project_name" == "opencog" ]; then
            # OpenCog depends on atomspace, cogserver, etc.
            dependencies+=("cogutil" "atomspace" "atomspace-storage" "atomspace-rocks" "cogserver")
        elif [ "$project_name" == "learn" ]; then
            # Learn depends on atomspace
            dependencies+=("cogutil" "atomspace")
        elif [ "$project_name" == "ure" ]; then
            # URE depends on atomspace
            dependencies+=("cogutil" "atomspace")
        elif [ "$project_name" == "attention" ]; then
            # Attention depends on atomspace, cogserver
            dependencies+=("cogutil" "atomspace" "cogserver")
        elif [ "$project_name" == "sensory" ]; then
            # Sensory depends on atomspace
            dependencies+=("cogutil" "atomspace")
        fi
    fi
    
    echo "${dependencies[@]}"
}

# Function to update the build timestamp of a component
function update_build_timestamp {
    local component_dir=$1
    local state_file="$STATE_DIR/${component_dir//\//_}.timestamp"
    
    date +%s > "$state_file"
}

# Function to build a component
function build_component {
    local component_name=$1
    local component_dir=$2
    local force_build=$3
    local log_file="$LOGS_DIR/${component_name}_build.log"
    
    # Check if component has changed or if force build is requested
    if [ "$force_build" != "true" ] && ! has_component_changed "$component_dir"; then
        echo -e "${GREEN}Component $component_name is up to date, skipping.${NC}"
        return 0
    fi
    
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
    ldconfig >> $log_file 2>&1 || echo "Warning: Failed to run ldconfig after installing $component_name."
    
    # Update build timestamp
    cd ../../
    update_build_timestamp "$component_dir"
    
    echo -e "${GREEN}Successfully built and installed $component_name.${NC}"
}

# Function to display build status for all components
function display_build_status {
    echo -e "${BLUE}Build Status:${NC}"
    echo -e "${BLUE}------------${NC}"
    
    local components=("cogutil" "atomspace" "atomspace-storage" "atomspace-rocks" "atomspace-pgres" "cogserver" "learn" "opencog" "sensory" "evidence")
    
    for component in "${components[@]}"; do
        local state_file="$STATE_DIR/${component}.timestamp"
        if [ -f "$state_file" ]; then
            local last_build_time=$(cat "$state_file")
            local last_build_date=$(date -d "@$last_build_time" "+%Y-%m-%d %H:%M:%S")
            if has_component_changed "$component"; then
                echo -e "$component: ${YELLOW}Needs rebuild${NC} (Last built: $last_build_date)"
            else
                echo -e "$component: ${GREEN}Up to date${NC} (Last built: $last_build_date)"
            fi
        else
            echo -e "$component: ${RED}Never built${NC}"
        fi
    done
}

# Main build process
function incremental_build {
    section_header "Analyzing components"
    
    # List of all components in the required build order
    local components=(
        "cogutil"
        "atomspace"
        "unify"
        "ure"
        "atomspace-storage"
        "atomspace-rocks"
        "atomspace-pgres"
        "cogserver"
        "attention"
        "spacetime"
        "pln"
        "lg-atomese"
        "learn"
        "opencog"
        "sensory"
        "evidence"
    )
    
    local components_to_build=()
    
    # Check which components need to be rebuilt
    for component in "${components[@]}"; do
        if has_component_changed "$component"; then
            components_to_build+=("$component")
        fi
    done
    
    if [ ${#components_to_build[@]} -eq 0 ]; then
        echo -e "${GREEN}All components are up to date. Nothing to build.${NC}"
        return 0
    fi
    
    echo -e "${YELLOW}The following components need to be rebuilt:${NC}"
    for component in "${components_to_build[@]}"; do
        echo "  - $component"
    done
    
    # Build components in order
    section_header "Building components"
    for component in "${components[@]}"; do
        # Only build if component needs to be rebuilt
        if [[ " ${components_to_build[*]} " == *" $component "* ]]; then
            build_component "$component" "$component" false
        fi
    done
    
    echo -e "${GREEN}Incremental build completed successfully!${NC}"
}

# Parse command line arguments
case "$1" in
    --status)
        display_build_status
        exit 0
        ;;
    --rebuild-all)
        # Force rebuild all components
        section_header "Rebuilding all components"
        local components=(
            "cogutil"
            "atomspace"
            "unify"
            "ure"
            "atomspace-storage"
            "atomspace-rocks"
            "atomspace-pgres"
            "cogserver"
            "attention"
            "spacetime"
            "pln"
            "lg-atomese"
            "learn"
            "opencog"
            "sensory"
            "evidence"
        )
        for component in "${components[@]}"; do
            build_component "$component" "$component" true
        done
        echo -e "${GREEN}Full rebuild completed successfully!${NC}"
        exit 0
        ;;
    --rebuild)
        if [ -z "$2" ]; then
            error_exit "Please specify a component to rebuild."
        fi
        build_component "$2" "$2" true
        exit 0
        ;;
    --help|-h)
        echo "Usage: $0 [OPTION]"
        echo "Build OpenCog components incrementally."
        echo ""
        echo "Options:"
        echo "  --status            Display build status of all components"
        echo "  --rebuild-all       Force rebuild of all components"
        echo "  --rebuild COMPONENT Force rebuild of a specific component"
        echo "  --help, -h          Display this help and exit"
        echo ""
        exit 0
        ;;
    *)
        incremental_build
        ;;
esac

exit 0 