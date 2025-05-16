#!/bin/bash
#
# OpenCog Basic Components Cloner
# This script clones only the essential OpenCog repositories
#

set -e  # Exit on error

echo "OpenCog Basic Components Cloner"
echo "==============================="
echo

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Function to display error and exit
function error_exit {
    echo -e "${RED}$1${NC}" 1>&2
    exit 1
}

# Create logs directory
LOGS_DIR="clone_logs"
mkdir -p $LOGS_DIR

# Create directory for the standalone repository
STANDALONE_DIR="opencog-basic"
mkdir -p $STANDALONE_DIR

# Function to clone a repository
function clone_repo {
    local repo_url=$1
    local branch=$2
    local dest_dir=$3
    
    if [ -z "$dest_dir" ]; then
        # Extract repository name from URL
        dest_dir=$(basename $repo_url | sed 's/\.git$//')
    fi
    
    if [ -z "$branch" ]; then
        branch="master"
    fi
    
    echo -e "${YELLOW}Cloning $repo_url ($branch) into $STANDALONE_DIR/$dest_dir...${NC}"
    
    if [ -d "$STANDALONE_DIR/$dest_dir" ]; then
        echo -e "${YELLOW}$dest_dir already exists. Updating...${NC}"
        (cd $STANDALONE_DIR/$dest_dir && git pull origin $branch) >> "$LOGS_DIR/${dest_dir}_clone.log" 2>&1 || error_exit "Failed to update $dest_dir"
    else
        git clone --branch $branch $repo_url $STANDALONE_DIR/$dest_dir >> "$LOGS_DIR/${dest_dir}_clone.log" 2>&1 || error_exit "Failed to clone $repo_url"
    fi
    
    echo -e "${GREEN}Successfully cloned/updated $dest_dir${NC}"
}

# Read repositories from repos-basic.txt
if [ ! -f "repos-basic.txt" ]; then
    error_exit "repos-basic.txt file not found!"
fi

echo "Cloning basic components into $STANDALONE_DIR..."
while read -r line || [ -n "$line" ]; do
    # Skip comments and empty lines
    if [[ $line == \#* ]] || [[ -z $line ]]; then
        continue
    fi
    
    # Parse line: URL [BRANCH]
    read -r repo_url branch <<< "$line"
    
    # Clone the repository
    clone_repo "$repo_url" "$branch"
done < "repos-basic.txt"

# Copy essential build files to the standalone directory
echo -e "${YELLOW}Copying build system files...${NC}"
cp install-dependencies.sh build.sh $STANDALONE_DIR/ 2>/dev/null || echo "Warning: Some build files not found, skipping."

# Create a basic CMakeLists.txt if it doesn't exist
if [ ! -f "$STANDALONE_DIR/CMakeLists.txt" ]; then
    echo -e "${YELLOW}Creating basic CMakeLists.txt...${NC}"
    cat > $STANDALONE_DIR/CMakeLists.txt << 'EOF'
# OpenCog Basic Components
# CMake build file for minimal OpenCog setup

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(opencog-basic)

# Default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

# Add each component subdirectory
ADD_SUBDIRECTORY(cogutil)
ADD_SUBDIRECTORY(atomspace)
ADD_SUBDIRECTORY(atomspace-storage)
ADD_SUBDIRECTORY(atomspace-rocks)
ADD_SUBDIRECTORY(cogserver)

MESSAGE(STATUS "OpenCog basic components build configured")
EOF
fi

# Create a basic README.md
echo -e "${YELLOW}Creating README.md...${NC}"
cat > $STANDALONE_DIR/README.md << 'EOF'
# OpenCog Basic Components

This is a standalone repository containing only the essential core components of the OpenCog framework.

## Components Included

1. **cogutil** - Utility library with common functions and data structures
2. **atomspace** - Core knowledge representation framework
3. **atomspace-storage** - Persistence API for AtomSpace
4. **atomspace-rocks** - RocksDB backend for AtomSpace persistence
5. **cogserver** - Network server providing AtomSpace API access

## Building

### Prerequisites

- C++17 compatible compiler (gcc 7.5+, clang 6+, or MSVC 2019+)
- CMake 3.12+
- Boost 1.68+
- Python 3.6+ with development headers

### Linux/macOS Build

```bash
# Install dependencies
./install-dependencies.sh

# Build
./build.sh
```

### Windows Build

```powershell
# Install dependencies
./install-dependencies.ps1

# Build
./build.ps1
```

## Usage

After building, you can use the components individually or together as a cohesive system.

Basic example to start a CogServer:

```bash
./opencog-basic/cogserver/build/opencog/cogserver/server/cogserver
```

## Advanced Components

For advanced functionality, you may want to add additional components:
- atomspace-pgres - PostgreSQL backend for AtomSpace persistence
- unify - Pattern matching and unification
- ure - Rule engine for inference

These can be enabled by uncommenting the respective entries in repos-basic.txt
and running the clone script again.
EOF

echo
echo -e "${GREEN}Basic OpenCog components cloned successfully to $STANDALONE_DIR!${NC}"
echo "See $LOGS_DIR/ for detailed clone logs."
echo
echo -e "${YELLOW}Next steps:${NC}"
echo "1. cd $STANDALONE_DIR"
echo "2. ./install-dependencies.sh  # Install prerequisites"
echo "3. ./build.sh                 # Build all components" 