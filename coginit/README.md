# CogInit - OpenCog Prerequisites Manager

CogInit is a comprehensive tool for managing all prerequisites and dependencies needed for building OpenCog components. It simplifies the setup process by providing local copies of source code, automated installation scripts, and verification tests.

## Overview

Setting up the OpenCog development environment can be complex due to numerous dependencies. CogInit addresses this by:

1. **Centralizing dependency management** - All required dependencies are defined in a single JSON file
2. **Providing local source copies** - Eliminating network issues during installation
3. **Automating installation** - Cross-platform scripts handle the heavy lifting
4. **Verifying setup** - Ensuring all components are installed correctly
5. **Documenting build processes** - Clear instructions for building from source

## Contents

- **deps/** - Local source code copies of all dependencies
- **scripts/** - Installation scripts for different platforms
- **tests/** - Verification tests for each dependency
- **docs/** - Comprehensive documentation

## Getting Started

### On Windows:

```powershell
# Install all dependencies
.\scripts\install-windows.ps1

# For essential dependencies only
.\scripts\install-windows.ps1 -Essential

# Install without prompting
.\scripts\install-windows.ps1 -All -NoPrompt

# Verify installation
.\scripts\verify-windows.ps1
```

### On Linux:

```bash
# Install all dependencies
./scripts/install-linux.sh

# For essential dependencies only
./scripts/install-linux.sh --essential

# Install without prompting
./scripts/install-linux.sh --all --no-prompt

# Verify installation
./scripts/verify-linux.sh
```

## Essential Dependencies

1. **Boost** - C++ utilities package
   - Required for many OpenCog components
   - Version 1.68.0 or higher recommended

2. **CMake** - Build management tool
   - Required for building all components
   - Version 3.12.0 or higher

3. **CxxTest** - Unit test framework
   - Required for running tests
   - Breaking unit tests is verboten!

4. **Guile** - Embedded scheme REPL
   - Core requirement for AtomSpace
   - Version 3.0 or newer required

5. **OpenSSL** - SSL/TLS implementation
   - Required for WebSockets in CogServer
   - Development headers needed

## Optional Dependencies

1. **Binutils BFD** - GNU Binary File Description library
   - Improves stack traces for debugging
   - Recommended for developers

2. **Iberty** - GCC compiler tools component
   - Works with BFD for better stack traces
   - Part of GCC toolchain

3. **Doxygen** - Documentation generator
   - Generates code documentation
   - Useful for exploring the codebase

## Building from Source

For some dependencies, binary packages may not be available or may be outdated. In these cases, CogInit provides:

1. Source code archives in the `deps/` directory
2. Detailed build instructions in the `docs/` directory
3. Helper scripts to compile from source

See the specific documentation files (e.g., `docs/boost_windows.md` or `docs/boost_linux.md`) for detailed instructions.

## Customizing Installation

Edit the `dependencies.json` file to add or modify dependencies. Each entry requires:

```json
{
  "Name": "package-name",
  "Version": "x.y.z",
  "Url": "https://download/url/package-x.y.z.tar.gz",
  "FileName": "package-x.y.z.tar.gz",
  "Description": "Package description",
  "Essential": true/false
}
```

## Troubleshooting

If you encounter issues:

1. Check the verification output with `verify-windows.ps1` or `verify-linux.sh`
2. Review logs in the `downloads_logs/` directory
3. See if manual installation steps are needed in `docs/` directory
4. For build failures, try building manually following the instructions
