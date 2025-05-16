# CogInit - OpenCog Prerequisites Manager

CogInit is a tool for managing all prerequisites and dependencies needed for building OpenCog components.
It includes local copies of source code, installation scripts, and verification tests.

## Contents

- **deps/** - Local source code copies of all dependencies
- **scripts/** - Installation scripts for different platforms
- **tests/** - Verification tests for each dependency
- **docs/** - Comprehensive documentation

## Getting Started

### On Windows:

`powershell
# Install all dependencies
.\scripts\install-windows.ps1

# Verify installation
.\scripts\verify-windows.ps1
`

### On Linux:

`ash
# Install all dependencies
./scripts/install-linux.sh

# Verify installation
./scripts/verify-linux.sh
`

## Essential Dependencies

1. **Boost** - C++ utilities package
2. **CMake** - Build management tool
3. **CxxTest** - Unit test framework
4. **Guile** - Embedded scheme REPL
5. **OpenSSL** - SSL/TLS implementation

## Optional Dependencies

1. **Binutils BFD** - GNU Binary File Description library 
2. **Iberty** - GCC compiler tools component
3. **Doxygen** - Documentation generator

## Customizing Installation

Edit the \dependencies.json\ file to add or modify dependencies.
