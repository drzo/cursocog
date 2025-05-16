# OpenCog Basic Components

This is a standalone repository containing only the essential core components of the OpenCog framework.

## Components Included

1. **cogutil** - Utility library with common functions and data structures
2. **atomspace** - Core knowledge representation framework
3. **atomspace-storage** - Persistence API for AtomSpace
4. **atomspace-rocks** - RocksDB backend for AtomSpace persistence
5. **cogserver** - Network server providing AtomSpace API access
6. **clay-ui** - Visualization and interaction system

## Building

### Prerequisites

- C++17 compatible compiler (gcc 7.5+, clang 6+, or MSVC 2019+)
- CMake 3.12+
- Boost 1.68+
- Python 3.6+ with development headers
- OpenGL development libraries (for Clay UI)

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

## Learning Resources

### AtomSpace Examples

A collection of example scripts is provided to help you learn how to use AtomSpace:

- **Basic AtomSpace Examples**: Fundamental concepts and operations
- **RocksDB Persistence**: How to store and retrieve AtomSpace data
- **Pattern Matching**: Advanced querying and knowledge discovery

```bash
# Location of examples
cd examples/atomspace-examples

# Load an example in Guile
guile -l basic_atomspace.scm
```

See the [examples README](examples/atomspace-examples/README.md) for more details.

## Advanced Components

For advanced functionality, you may want to add additional components:
- atomspace-pgres - PostgreSQL backend for AtomSpace persistence
- unify - Pattern matching and unification
- ure - Rule engine for inference

These can be enabled by uncommenting the respective entries in repos-basic.txt
and running the clone script again.

## Graphical User Interface

OpenCog Basic includes Clay UI, a visualization and interaction system for OpenCog components:

### OpenCog Dashboard

The Dashboard provides a user-friendly interface for:
- Installing dependencies
- Building components
- Accessing tutorials
- Visualizing the AtomSpace

To launch the dashboard:

```bash
# On Linux
./opencog-dashboard.sh

# On Windows
opencog-dashboard.bat
```

### AtomSpace Viewer

A dedicated tool for visualizing and interacting with the AtomSpace:

```bash
# Build and run
cd clay-ui/build
cmake ..
cmake --build . --target atomspace-viewer
./atomspace-viewer
```

## Testing and Validation

OpenCog Basic includes comprehensive testing scripts to verify the proper integration and functionality of all components:

### Clay UI Testing

To validate the Clay UI integration:

```bash
# On Linux
./test-clay-ui.sh

# On Windows
.\test-clay-ui.ps1
```

These scripts perform the following checks:
- Verify all required dependencies are installed
- Check the Clay UI directory structure
- Test building the dashboard application
- Verify AtomSpace integration
- Test the launcher scripts

### Multi-Platform Testing

The project is tested on:
- Ubuntu Linux 20.04 and 22.04
- Windows 10 and 11
- macOS 10.15+

For platform-specific issues, see the documentation in the `doc` directory.

## Installation Packages

### Linux Packages

You can create DEB and RPM packages for Linux distributions:

```bash
# Create Linux packages
./create-packages.sh
```

This creates:
- DEB package for Debian/Ubuntu-based distributions
- RPM package for RedHat/Fedora-based distributions

The packages are created in the `build` directory.

### Windows Installer

For Windows users, an NSIS-based installer is available:

```powershell
# Create Windows installer
.\create-windows-installer.ps1
```

This creates a Windows installer in the `build` directory.

## Dependency Management with CogInit

This repository includes CogInit, a dependency management system for OpenCog. CogInit simplifies the setup process by providing:

1. Local copies of dependency source files
2. Automated installation scripts
3. Verification tools

### Using CogInit

To build with automatic dependency management:

```bash
# On Linux
./build-with-coginit.sh

# On Windows
.\build-with-coginit.ps1
```

These scripts will verify dependencies are installed before building and offer to install missing ones.

### Manual Dependency Management

You can also manage dependencies directly:

```bash
# Install dependencies on Linux
./coginit/scripts/install-linux.sh

# Install dependencies on Windows
.\coginit\scripts\install-windows.ps1
```

For more information, see the README in the [coginit](./coginit) directory.
