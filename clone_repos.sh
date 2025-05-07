#!/bin/bash
#
# OpenCog Repository Cloner
# This script clones all necessary OpenCog repositories for development
#

set -e  # Exit on error

echo "OpenCog Repository Cloner"
echo "========================="
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
    
    echo -e "${YELLOW}Cloning $repo_url ($branch) into $dest_dir...${NC}"
    
    if [ -d "$dest_dir" ]; then
        echo -e "${YELLOW}$dest_dir already exists. Updating...${NC}"
        (cd $dest_dir && git pull origin $branch) >> "$LOGS_DIR/${dest_dir}_clone.log" 2>&1 || error_exit "Failed to update $dest_dir"
    else
        git clone --branch $branch $repo_url $dest_dir >> "$LOGS_DIR/${dest_dir}_clone.log" 2>&1 || error_exit "Failed to clone $repo_url"
    fi
    
    echo -e "${GREEN}Successfully cloned/updated $dest_dir${NC}"
}

# Read repositories from repos.txt
if [ ! -f "repos.txt" ]; then
    error_exit "repos.txt file not found!"
fi

echo "Cloning repositories from repos.txt..."
while read -r line || [ -n "$line" ]; do
    # Skip comments and empty lines
    if [[ $line == \#* ]] || [[ -z $line ]]; then
        continue
    fi
    
    # Parse line: URL [BRANCH]
    read -r repo_url branch <<< "$line"
    
    # Clone the repository
    clone_repo "$repo_url" "$branch"
done < "repos.txt"

echo
echo -e "${GREEN}All repositories cloned successfully!${NC}"
echo "See $LOGS_DIR/ for detailed clone logs." 