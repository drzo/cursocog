#!/bin/bash
#
# OpenCog macOS Package Creation Script
# This script creates a DMG package for macOS installation
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

# Package configuration
APP_NAME="OpenCog"
VERSION=$(git describe --tags --always || echo "0.1.0")
DMG_NAME="${APP_NAME}-${VERSION}"
STAGING_DIR="${ROOT_DIR}/staging"
DMG_DIR="${ROOT_DIR}/packages"
RESOURCES_DIR="${SCRIPT_DIR}/resources/macos"

# Headers
echo -e "${BLUE}OpenCog macOS Package Creator${NC}"
echo -e "${BLUE}=============================${NC}"
echo -e "Creating package for version: ${GREEN}${VERSION}${NC}\n"

# Check for required tools
if ! command -v create-dmg >/dev/null 2>&1; then
    echo -e "${YELLOW}create-dmg not found. Installing...${NC}"
    brew install create-dmg || { echo -e "${RED}Failed to install create-dmg.${NC}"; exit 1; }
fi

# Clean previous builds
echo -e "${BLUE}Cleaning previous builds...${NC}"
rm -rf "${STAGING_DIR}" "${DMG_DIR}/${DMG_NAME}.dmg"
mkdir -p "${STAGING_DIR}/Applications" "${DMG_DIR}" "${RESOURCES_DIR}"

# Build OpenCog if not already built
if [ ! -d "${ROOT_DIR}/build" ]; then
    echo -e "${BLUE}Building OpenCog...${NC}"
    "${ROOT_DIR}/build.sh" || { echo -e "${RED}Build failed.${NC}"; exit 1; }
fi

# Copy application files
echo -e "${BLUE}Copying application files...${NC}"
mkdir -p "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/"{MacOS,Resources,Frameworks}

# Copy executable
cp "${ROOT_DIR}/clay-ui/build/opencog-dashboard" "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/MacOS/"
chmod +x "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/MacOS/opencog-dashboard"

# Create Info.plist
cat > "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>opencog-dashboard</string>
    <key>CFBundleIconFile</key>
    <string>opencog.icns</string>
    <key>CFBundleIdentifier</key>
    <string>org.opencog.dashboard</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>${APP_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>${VERSION}</string>
    <key>CFBundleVersion</key>
    <string>${VERSION}</string>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
EOF

# Create application icon if not exists
if [ ! -f "${RESOURCES_DIR}/opencog.icns" ]; then
    echo -e "${YELLOW}Icon file not found. Using default icon.${NC}"
    # Copy a default icon or create a placeholder
    # In a real scenario, you would have a proper icon file
    touch "${RESOURCES_DIR}/opencog.icns"
fi

# Copy icon
cp "${RESOURCES_DIR}/opencog.icns" "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/Resources/"

# Copy libraries and resources
echo -e "${BLUE}Copying libraries and resources...${NC}"
# Copy libraries and dependencies
mkdir -p "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/lib"
cp -R "${ROOT_DIR}/build/lib/"* "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/Frameworks/" 2>/dev/null || true

# Copy examples and documentation
mkdir -p "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/Resources/Examples"
mkdir -p "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/Resources/Documentation"
cp -R "${ROOT_DIR}/examples/"* "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/Resources/Examples/" 2>/dev/null || true
cp "${ROOT_DIR}/README.md" "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/Resources/Documentation/"

# Create uninstaller script
mkdir -p "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/Resources/Scripts"
cat > "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/Resources/Scripts/uninstall.sh" << 'EOF'
#!/bin/bash
# Uninstall OpenCog
APP_PATH="/Applications/OpenCog.app"

echo "Uninstalling OpenCog..."
rm -rf "$APP_PATH"
echo "OpenCog has been uninstalled."
EOF
chmod +x "${STAGING_DIR}/Applications/${APP_NAME}.app/Contents/Resources/Scripts/uninstall.sh"

# Create README file for the DMG
cat > "${STAGING_DIR}/README.txt" << EOF
OpenCog ${VERSION}

To install:
1. Drag the OpenCog application to the Applications folder.
2. Double click to start the OpenCog dashboard.

For more information, visit: https://opencog.org
EOF

# Create license file for the DMG
cp "${ROOT_DIR}/LICENSE" "${STAGING_DIR}/License.txt" 2>/dev/null || echo "OpenCog is licensed under AGPLv3." > "${STAGING_DIR}/License.txt"

# Create the DMG
echo -e "${BLUE}Creating DMG package...${NC}"
create-dmg \
    --volname "${APP_NAME} ${VERSION}" \
    --volicon "${RESOURCES_DIR}/opencog.icns" \
    --window-pos 200 120 \
    --window-size 800 450 \
    --icon-size 100 \
    --icon "${APP_NAME}.app" 200 190 \
    --app-drop-link 600 185 \
    --hide-extension "${APP_NAME}.app" \
    --eula "${STAGING_DIR}/License.txt" \
    --add-file "README.txt" "${STAGING_DIR}/README.txt" 400 320 \
    "${DMG_DIR}/${DMG_NAME}.dmg" \
    "${STAGING_DIR}/Applications" || { echo -e "${RED}Failed to create DMG.${NC}"; exit 1; }

# Clean up
echo -e "${BLUE}Cleaning up...${NC}"
rm -rf "${STAGING_DIR}"

echo -e "${GREEN}Package created successfully: ${DMG_DIR}/${DMG_NAME}.dmg${NC}"
echo -e "File size: $(du -h "${DMG_DIR}/${DMG_NAME}.dmg" | cut -f1)" 