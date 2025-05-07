#!/bin/bash
#
# Script to download Clay UI library
#

set -e  # Exit on error

echo "Downloading Clay UI library..."
mkdir -p src
cd src
curl -L -o clay.h https://raw.githubusercontent.com/nicbarker/clay/main/clay.h
echo "Downloaded Clay UI library to src/clay.h"

# Download an example renderer
mkdir -p renderers
cd renderers
curl -L -o gl_renderer.h https://raw.githubusercontent.com/nicbarker/clay/main/renderers/opengl/gl_renderer.h
echo "Downloaded OpenGL renderer to src/renderers/gl_renderer.h"

# Return to the root directory
cd ../..
echo "Clay UI download complete!" 