# OpenCog Basic Build Instructions for Windows

This guide provides step-by-step instructions for building OpenCog Basic on Windows.

## Prerequisites

Before building OpenCog, ensure you have the following installed:

1. **Visual Studio 2019 or newer** with C++ development tools
2. **CMake 3.10 or newer** (added to PATH)
3. **Boost Libraries 1.68 or newer** (via vcpkg) 
4. **Python 3.7 or newer** with Cython and NumPy
5. **Git** for Windows

## Installation Steps

### 1. Install Dependencies

```powershell
# Run as Administrator
.\install-dependencies.ps1
```

If you don't have admin rights, you can install dependencies manually:

```powershell
# Install vcpkg (skip if already installed)
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat -disableMetrics
.\vcpkg integrate install

# Install required libraries
.\vcpkg install boost-filesystem:x64-windows boost-program-options:x64-windows boost-system:x64-windows boost-thread:x64-windows
.\vcpkg install boost-math:x64-windows boost-random:x64-windows boost-serialization:x64-windows
.\vcpkg install rocksdb:x64-windows libpq:x64-windows sqlite3:x64-windows

# Set VCPKG_ROOT environment variable
$env:VCPKG_ROOT = "C:\vcpkg"
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\vcpkg", [System.EnvironmentVariableTarget]::User)

# Install Python packages
pip install cython numpy
```

### 2. Build Components in Order

OpenCog components need to be built in a specific order due to dependencies:

#### 2.1 Build CogUtil

```powershell
# Clone if not already done
git clone https://github.com/opencog/cogutil.git
cd cogutil
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows

# Build and install
cmake --build . --config Release
cmake --install .

# Return to root directory
cd ../..
```

#### 2.2 Build AtomSpace

```powershell
# Clone if not already done
git clone https://github.com/opencog/atomspace.git
cd atomspace
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows

# Build and install
cmake --build . --config Release
cmake --install .

# Return to root directory
cd ../..
```

#### 2.3 Build AtomSpace Storage Backends

```powershell
# Build AtomSpace Storage
git clone https://github.com/opencog/atomspace-storage.git
cd atomspace-storage
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build . --config Release
cmake --install .
cd ../..

# Build AtomSpace RocksDB
git clone https://github.com/opencog/atomspace-rocks.git
cd atomspace-rocks
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build . --config Release
cmake --install .
cd ../..
```

#### 2.4 Build CogServer

```powershell
git clone https://github.com/opencog/cogserver.git
cd cogserver
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build . --config Release
cmake --install .
cd ../..
```

#### 2.5 Build Clay UI (Optional)

```powershell
git clone https://github.com/opencog/clay-ui.git
cd clay-ui
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build . --config Release
cd ../..
```

## Running OpenCog

### Starting CogServer

```powershell
# Navigate to the cogserver executable location
cd "C:\Program Files\OpenCog\bin"
# or if installed to user directory
# cd "$env:USERPROFILE\OpenCog\bin"

# Start the cogserver
.\cogserver.exe
```

### Using Python Bindings

```python
# Python example
from opencog.atomspace import AtomSpace, TruthValue
from opencog.atomspace import types
from opencog.type_constructors import *
from opencog.utilities import initialize_opencog

# Initialize AtomSpace
atomspace = AtomSpace()
initialize_opencog(atomspace)

# Create some atoms
node1 = ConceptNode("cat")
node2 = ConceptNode("animal")
link = InheritanceLink(node1, node2)

# Print the AtomSpace
print(atomspace)
```

### Running Clay UI

```powershell
# Navigate to the Clay UI executable location
cd clay-ui\build
.\opencog-atomspace-viewer.exe
```

## Troubleshooting

### Common Issues

1. **CMake can't find CogUtil/AtomSpace**:
   Make sure you've installed each component before building the next one.

2. **Link errors with boost libraries**:
   Ensure you're using the correct Boost version and that it's properly linked in CMake.

3. **Python bindings not found**:
   Check that your PYTHONPATH includes the installed OpenCog Python modules.

4. **DLL load errors**:
   Make sure all required DLLs are in your system PATH or in the same directory as the executable.

### Getting Help

If you encounter issues, check the OpenCog wiki or GitHub repositories for more detailed information and troubleshooting guides.

## References

- OpenCog GitHub: https://github.com/opencog
- AtomSpace Documentation: https://wiki.opencog.org/w/AtomSpace
- CogServer Documentation: https://github.com/opencog/cogserver/README.md 