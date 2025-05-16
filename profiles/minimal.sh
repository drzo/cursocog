#!/bin/bash
#
# Minimal Build Profile for OpenCog
# This profile includes only core components with minimal features
#

# Define component-specific build options
declare -A COMPONENT_CONFIG

# Minimal configuration for each component
COMPONENT_CONFIG["cogutil"]="-DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF"
COMPONENT_CONFIG["atomspace"]="-DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF"
COMPONENT_CONFIG["atomspace-storage"]="-DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF"
COMPONENT_CONFIG["atomspace-rocks"]="-DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF"
COMPONENT_CONFIG["atomspace-pgres"]="-DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF"
COMPONENT_CONFIG["cogserver"]="-DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF -DENABLE_NETWORK_SERVER=ON"
COMPONENT_CONFIG["learn"]="-DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF"
COMPONENT_CONFIG["opencog"]="-DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF -DENABLE_LEARNING=OFF -DENABLE_PLN=OFF -DENABLE_ATTENTION=OFF"
COMPONENT_CONFIG["sensory"]="-DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF"
COMPONENT_CONFIG["evidence"]="-DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF"

# Components to skip in minimal build
SKIP_COMPONENTS=("unify" "ure" "attention" "spacetime" "pln" "lg-atomese")

echo "Minimal build profile loaded."
echo "This profile includes only core components with minimal features."
echo "The following components will be skipped: ${SKIP_COMPONENTS[*]}" 