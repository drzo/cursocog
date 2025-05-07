#!/bin/bash
#
# OpenCog Dependencies Installer
# This script installs all dependencies required to build OpenCog
#

set -e  # Exit on error

echo "OpenCog Dependencies Installer"
echo "=============================="
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

# Check if running as root or with sudo
if [ "$EUID" -ne 0 ]; then
    error_exit "Please run as root or with sudo."
fi

# Check if running on Debian/Ubuntu
if [ ! -f /etc/debian_version ]; then
    error_exit "This script is only for Debian/Ubuntu systems. For other systems, please install dependencies manually."
fi

# Update package lists
echo -e "${YELLOW}Updating package lists...${NC}"
apt-get update || error_exit "Failed to update package lists."

# Essential build tools
echo -e "${YELLOW}Installing essential build tools...${NC}"
apt-get install -y \
    build-essential \
    cmake \
    cxxtest \
    git \
    valgrind \
    libboost-dev \
    libboost-filesystem-dev \
    libboost-program-options-dev \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-math-dev \
    || error_exit "Failed to install essential build tools."

# Stack trace debugging tools
echo -e "${YELLOW}Installing debugging tools...${NC}"
apt-get install -y \
    binutils-dev \
    libiberty-dev \
    || error_exit "Failed to install debugging tools."

# Guile Scheme interpreter
echo -e "${YELLOW}Installing Guile Scheme interpreter...${NC}"
apt-get install -y \
    guile-3.0-dev \
    || error_exit "Failed to install Guile."

# Python and Cython for Python bindings
echo -e "${YELLOW}Installing Python and Cython...${NC}"
apt-get install -y \
    python3-dev \
    python3-pip \
    cython3 \
    || error_exit "Failed to install Python and Cython."

# Storage backend dependencies
echo -e "${YELLOW}Installing storage backend dependencies...${NC}"
apt-get install -y \
    libpq-dev \
    postgresql \
    postgresql-client \
    librocksdb-dev \
    || error_exit "Failed to install storage backend dependencies."

# Documentation tools
echo -e "${YELLOW}Installing documentation tools...${NC}"
apt-get install -y \
    doxygen \
    graphviz \
    || error_exit "Failed to install documentation tools."

# OCaml (optional)
echo -e "${YELLOW}Installing OCaml (optional bindings)...${NC}"
apt-get install -y \
    ocaml \
    ocaml-findlib \
    || echo -e "${YELLOW}OCaml installation failed, but it's optional.${NC}"

# Haskell (optional)
echo -e "${YELLOW}Installing Haskell (optional bindings)...${NC}"
apt-get install -y \
    ghc \
    cabal-install \
    || echo -e "${YELLOW}Haskell installation failed, but it's optional.${NC}"

# Link Grammar
echo -e "${YELLOW}Installing Link Grammar dependencies...${NC}"
apt-get install -y \
    autoconf-archive \
    automake \
    libtool \
    flex \
    zlib1g-dev \
    libsqlite3-dev \
    || error_exit "Failed to install Link Grammar dependencies."

# Install Python packages
echo -e "${YELLOW}Installing Python packages...${NC}"
pip3 install \
    pytest \
    nose \
    scipy \
    numpy \
    || error_exit "Failed to install Python packages."

echo -e "${GREEN}All dependencies have been installed successfully!${NC}"
echo 
echo "You can now clone and build the OpenCog repositories:"
echo "1. Run the build.sh script to build all components"
echo "2. Or build individual components as needed"
echo 
echo "For Windows users, see the build.ps1 script and vcpkg for dependencies."

exit 0 