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
