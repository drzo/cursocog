#!/bin/bash
#
# OpenCog Basic - Package Creation Script
# This script creates distribution packages (DEB and RPM) for OpenCog Basic
#

set -e  # Exit on error

echo "OpenCog Basic - Package Creation"
echo "==============================="
echo

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Verify requirements
check_requirements() {
    echo -e "${BLUE}Checking requirements...${NC}"
    local missing=0
    
    # Check for package building tools
    for tool in dpkg-deb rpmbuild fakeroot; do
        if ! command -v $tool &> /dev/null; then
            echo -e "${RED}Missing required tool: $tool${NC}"
            missing=1
        fi
    done
    
    if [ $missing -eq 1 ]; then
        echo -e "${RED}Please install the missing tools:${NC}"
        echo -e "${YELLOW}sudo apt-get install dpkg-dev rpm fakeroot${NC}"
        return 1
    fi
    
    echo -e "${GREEN}All requirements satisfied.${NC}"
    return 0
}

# Create a DEB package
create_deb_package() {
    echo -e "${BLUE}Creating DEB package...${NC}"
    
    # Create package directory structure
    local PKG_DIR="build/opencog-basic_0.1.0_amd64"
    local INSTALL_DIR="$PKG_DIR/usr/local/opencog-basic"
    
    rm -rf $PKG_DIR
    mkdir -p $PKG_DIR/DEBIAN
    mkdir -p $INSTALL_DIR
    mkdir -p $PKG_DIR/usr/local/bin
    mkdir -p $PKG_DIR/usr/share/applications
    mkdir -p $PKG_DIR/usr/share/doc/opencog-basic
    
    # Create control file
    cat > $PKG_DIR/DEBIAN/control << EOF
Package: opencog-basic
Version: 0.1.0
Section: science
Priority: optional
Architecture: amd64
Depends: libboost-all-dev, cmake, g++, python3
Maintainer: OpenCog Team <opencog@googlegroups.com>
Description: OpenCog Basic - Core components of the OpenCog framework
 OpenCog Basic includes the essential components for working with the
 AtomSpace knowledge representation framework and related tools.
 .
 Components included:
  - AtomSpace
  - AtomSpace Storage (RocksDB backend)
  - CogServer
  - Clay UI visualization
EOF
    
    # Copy files
    echo -e "${YELLOW}Copying files...${NC}"
    
    # Copy binaries (assuming they are built)
    mkdir -p $INSTALL_DIR/bin
    for bin in cogserver atomspace-rocks-client clay-ui/build/dashboard clay-ui/build/atomspace_viewer; do
        if [ -f "build/$bin" ]; then
            cp "build/$bin" $INSTALL_DIR/bin/
        elif [ -f "$bin" ]; then
            cp "$bin" $INSTALL_DIR/bin/
        fi
    done
    
    # Copy examples
    mkdir -p $INSTALL_DIR/examples
    cp -r examples/* $INSTALL_DIR/examples/
    
    # Copy launcher scripts
    cp opencog-dashboard.sh $INSTALL_DIR/bin/
    chmod +x $INSTALL_DIR/bin/opencog-dashboard.sh
    
    # Create symlinks in /usr/local/bin
    ln -sf /usr/local/opencog-basic/bin/opencog-dashboard.sh $PKG_DIR/usr/local/bin/opencog-dashboard
    
    # Create desktop entry
    cat > $PKG_DIR/usr/share/applications/opencog-dashboard.desktop << EOF
[Desktop Entry]
Name=OpenCog Dashboard
Comment=Dashboard for OpenCog Basic
Exec=/usr/local/bin/opencog-dashboard
Terminal=false
Type=Application
Categories=Science;Education;
EOF
    
    # Copy documentation
    cp README.md $PKG_DIR/usr/share/doc/opencog-basic/
    
    # Set permissions
    find $PKG_DIR -type d -exec chmod 755 {} \;
    find $PKG_DIR -type f -exec chmod 644 {} \;
    find $PKG_DIR/usr/local/opencog-basic/bin -type f -exec chmod 755 {} \;
    chmod 755 $PKG_DIR/usr/local/bin/opencog-dashboard
    
    # Build the package
    echo -e "${YELLOW}Building DEB package...${NC}"
    fakeroot dpkg-deb --build $PKG_DIR build/
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}DEB package created: build/opencog-basic_0.1.0_amd64.deb${NC}"
        return 0
    else
        echo -e "${RED}Failed to create DEB package${NC}"
        return 1
    fi
}

# Create an RPM package
create_rpm_package() {
    echo -e "${BLUE}Creating RPM package...${NC}"
    
    # Create spec file
    local SPEC_DIR="build/rpm"
    mkdir -p $SPEC_DIR
    
    cat > $SPEC_DIR/opencog-basic.spec << EOF
Name:           opencog-basic
Version:        0.1.0
Release:        1%{?dist}
Summary:        Core components of the OpenCog framework

License:        AGPLv3
URL:            https://github.com/opencog/opencog-basic

BuildRequires:  cmake gcc-c++ boost-devel
Requires:       boost python3

%description
OpenCog Basic includes the essential components for working with the
AtomSpace knowledge representation framework and related tools.

Components included:
 - AtomSpace
 - AtomSpace Storage (RocksDB backend)
 - CogServer
 - Clay UI visualization

%files
/usr/local/opencog-basic
/usr/local/bin/opencog-dashboard
/usr/share/applications/opencog-dashboard.desktop
/usr/share/doc/opencog-basic/README.md

%changelog
* $(date +"%a %b %d %Y") OpenCog Team <opencog@googlegroups.com> - 0.1.0-1
- Initial package release
EOF
    
    echo -e "${YELLOW}Building RPM package...${NC}"
    rpmbuild -bb --define "_topdir $(pwd)/build/rpm" $SPEC_DIR/opencog-basic.spec
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}RPM package created in build/rpm/RPMS/${NC}"
        return 0
    else
        echo -e "${RED}Failed to create RPM package${NC}"
        return 1
    fi
}

# Create output directory
mkdir -p build

# Main execution flow
check_requirements
req_status=$?

if [ $req_status -eq 0 ]; then
    create_deb_package
    deb_status=$?
    
    create_rpm_package
    rpm_status=$?
    
    # Summary
    echo
    echo -e "${BLUE}====== Package Creation Summary ======${NC}"
    echo
    echo -e "DEB package: $([ $deb_status -eq 0 ] && echo -e "${GREEN}SUCCESS${NC}" || echo -e "${RED}FAILED${NC}")"
    echo -e "RPM package: $([ $rpm_status -eq 0 ] && echo -e "${GREEN}SUCCESS${NC}" || echo -e "${RED}FAILED${NC}")"
    echo
    
    if [ $deb_status -eq 0 ] || [ $rpm_status -eq 0 ]; then
        echo -e "${GREEN}Packages created in the 'build' directory.${NC}"
    else
        echo -e "${RED}Failed to create packages.${NC}"
        exit 1
    fi
else
    echo -e "${RED}Cannot create packages due to missing requirements.${NC}"
    exit 1
fi 