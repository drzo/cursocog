# OpenCog Dependency Verification Tool

This tool checks for all dependencies required to build and run OpenCog projects, verifies their versions, and reports any issues found.

## Overview

The dependency verification tool:

- Detects platform-specific dependencies across Windows, Linux, and macOS
- Verifies dependency versions meet minimum requirements
- Provides installation instructions for missing or outdated dependencies
- Offers both command-line output and JSON format for integration with other tools

## Usage

### Windows

Run the following command in PowerShell:

```powershell
.\scripts\verify-dependencies.ps1
```

Options:
- `-verbose` or `-v`: Enable verbose output
- `-json`: Output results in JSON format

### Linux/macOS

Run the following command in Bash:

```bash
./scripts/verify-dependencies.sh
```

Options:
- `--verbose` or `-v`: Enable verbose output
- `--json`: Output results in JSON format

## Examples

### Basic Check

Run a basic dependency check:

```bash
./scripts/verify-dependencies.sh
```

### JSON Output

Get results in JSON format (useful for integration with other tools):

```bash
./scripts/verify-dependencies.sh --json
```

### Verbose Output

Get detailed diagnostic information:

```bash
./scripts/verify-dependencies.sh --verbose
```

## Understanding Results

The tool categorizes dependencies as:

- **Required dependencies** - These must be installed with the minimum required version
- **Optional dependencies** - These enhance functionality but aren't strictly necessary

The output uses the following symbols:

- ✓ (Green): Dependency found and meets version requirements
- ⚠ (Yellow): Dependency found but doesn't meet minimum version requirements
- ✗ (Red): Dependency not found
- ○ (Yellow): Optional dependency not found

## Dependencies Checked

### Core Dependencies

- CMake (build system)
- Boost (C++ libraries)
- CxxTest (testing framework)
- Git (version control)
- Python (3.6+)
- Cython (Python extension)
- Doxygen (documentation generator)
- Guile (Scheme interpreter)

### Platform-Specific Dependencies

#### Linux
- GCC/G++ (C/C++ compiler)
- libpq (PostgreSQL client)
- RocksDB
- PostgreSQL

#### macOS
- Clang (C/C++ compiler)
- libpq (PostgreSQL client)
- RocksDB
- PostgreSQL

#### Windows
- Visual C++ (MSVC)
- vcpkg (package manager)

### Optional Dependencies

- OCaml
- Haskell (GHC)
- Graphviz

## Fixing Issues

If the tool reports missing or outdated dependencies, you can:

1. **For Linux** - Run the OpenCog dependency installer:
   ```bash
   sudo ./install-dependencies.sh
   ```

2. **For Windows** - Run the PowerShell installer:
   ```powershell
   # Run as Administrator
   .\install-dependencies.ps1
   ```

3. **For macOS** - Use Homebrew:
   ```bash
   brew install cmake boost cxxtest guile python3 rocksdb postgresql
   pip3 install cython pytest numpy scipy
   ```

## Integration

The tool can be integrated into build scripts to ensure dependencies are checked before building. For example:

```bash
# In your build script
if ! ./scripts/verify-dependencies.sh; then
    echo "Dependencies missing or outdated. Please install required dependencies."
    exit 1
fi

# Continue with build if dependencies are satisfied
./build.sh
```

## Technical Details

The verification tool is implemented in Python and works by:

1. Detecting the operating system
2. Checking for required and optional dependencies based on the platform
3. Verifying minimum versions where applicable
4. Providing installation instructions tailored to the platform

The tool can be extended to check for additional dependencies by modifying the Python script. 