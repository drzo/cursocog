#!/bin/bash
#
# CogInit - Linux Dependencies Verification
# Verifies all prerequisites for OpenCog development on Linux
#

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "CogInit - OpenCog Prerequisites Verification (Linux)"
echo -e "=================================================="
echo

# Function to compare versions
function version_compare() {
    if [[  ==  ]]; then
        return 0
    fi
    local IFS=.
    local i ver1=() ver2=()
    # Fill empty fields with zeros
    for ((i=; i<; i++)); do
        ver1[i]=0
    done
    for ((i=0; i<; i++)); do
        if [[ -z  ]]; then
            ver2[i]=0
        fi
        if ((10 > 10)); then
            return 1
        fi
        if ((10 < 10)); then
            return 2
        fi
    done
    return 0
}

# Function to check a dependency
function check_dependency() {
    local NAME=
    local COMMAND=
    local PATTERN=
    local MIN_VERSION=
    local REQUIRED=
    
    local RESULT=Not found
    local VERSION="N/A"
    local STATUS="Not found"
    
    if [[  != "Not found" ]]; then
        VERSION=
        STATUS="Installed"
        
        # Check version if needed
        if [[ -n  ]]; then
            version_compare  
            COMP_RESULT=True
            
            if [[  -eq 2 ]]; then
                STATUS="Version too old"
            fi
        fi
    fi
    
    # Format output based on status and required flag
    if [[  == "Installed" ]]; then
        printf "%-15s %-15s %-30s %-10s\n" "" "" "Green" ""
    elif [[  == "Yes" &&  != "Installed" ]]; then
        printf "%-15s %-15s %-30s %-10s\n" "" "" "Red" ""
    else
        printf "%-15s %-15s %-30s %-10s\n" "" "" "Yellow" ""
    fi
}

# Display table header
printf "%-15s %-15s %-30s %-10s\n" "Dependency" "Required" "Status" "Version"
printf "%-15s %-15s %-30s %-10s\n" "-----------" "--------" "------" "-------"

# Check each dependency
check_dependency "Boost" "dpkg -s libboost-dev | grep Version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "1.68.0" "Yes"
check_dependency "CMake" "cmake --version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "3.12.0" "Yes"
check_dependency "CxxTest" "dpkg -s cxxtest | grep Version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "4.0.0" "Yes"
check_dependency "Guile" "guile --version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "3.0.0" "Yes"
check_dependency "OpenSSL" "dpkg -s libssl-dev | grep Version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "1.0.0" "No"
check_dependency "Binutils" "dpkg -s binutils-dev | grep Version" "([0-9]+\\.[0-9]+)" "2.30" "No"
check_dependency "Doxygen" "doxygen --version" "([0-9]+\\.[0-9]+\\.[0-9]+)" "1.8.0" "No"

echo
echo "Verification complete. Make sure all required dependencies are installed."
echo "For any missing components, run ./scripts/install-linux.sh"
