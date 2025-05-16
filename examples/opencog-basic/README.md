# OpenCog Basic with Clay UI

This guide provides comprehensive instructions for installing and using OpenCog Basic with the Clay UI visualization tool. 

## Prerequisites

- Python 3.7+ with pip
- C++ compiler (GCC 5+ on Linux, Visual Studio 2019+ on Windows)
- CMake 3.10+
- Boost libraries 1.68+
- Git

## Installation

OpenCog can be installed either by building from source or using Python bindings. The full build from source is recommended for complete functionality.

### Option 1: Complete Build from Source (Recommended)

#### Linux/macOS

```bash
# Clone the repository
git clone https://github.com/opencog/opencog-basic
cd opencog-basic

# Install dependencies
./install-dependencies.sh

# Build and install the components
./build.sh --profile minimal
```

#### Windows

```powershell
# Clone the repository
git clone https://github.com/opencog/opencog-basic
cd opencog-basic

# Install dependencies
.\install-dependencies.ps1

# Build and install the components
.\build.ps1 -Profile minimal
```

### Option 2: Building Individual Components

If you prefer to build only specific components, you can do so as follows:

#### Linux/macOS

```bash
# Clone and build cogutil
git clone https://github.com/opencog/cogutil
cd cogutil
mkdir build
cd build
cmake ..
make -j
sudo make install
cd ../..

# Clone and build atomspace
git clone https://github.com/opencog/atomspace
cd atomspace
mkdir build
cd build
cmake ..
make -j
sudo make install
cd ../..

# Clone and build atomspace-rocks (for persistent storage)
git clone https://github.com/opencog/atomspace-rocks
cd atomspace-rocks
mkdir build
cd build
cmake ..
make -j
sudo make install
cd ../..

# Clone and build cogserver
git clone https://github.com/opencog/cogserver
cd cogserver
mkdir build
cd build
cmake ..
make -j
sudo make install
cd ../..

# Build Clay UI
git clone https://github.com/opencog/clay-ui
cd clay-ui
mkdir build
cd build
cmake ..
make -j
cd ../..
```

#### Windows

Windows builds are more complex due to dependency management. We recommend using vcpkg for managing C++ dependencies:

```powershell
# Install vcpkg if not already available
git clone https://github.com/microsoft/vcpkg
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install boost:x64-windows guile:x64-windows uuid:x64-windows

# Build OpenCog components following similar steps as Linux but using:
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build . --config Release
cmake --install .
```

### Option 3: Python Bindings Only (Limited Functionality)

If you only need the Python bindings for the AtomSpace (without visualization and advanced features):

```bash
# Install Cython first
pip install cython

# Install dependencies (Ubuntu/Debian)
sudo apt-get install libboost-all-dev libffi-dev

# Install AtomSpace Python bindings
git clone https://github.com/opencog/atomspace
cd atomspace
mkdir build
cd build
cmake -DPYTHON_EXECUTABLE=$(which python3) ..
make -j
sudo make install

# Set Python path (add to your .bashrc or .zshrc)
export PYTHONPATH=$PYTHONPATH:/usr/local/lib/python3.8/site-packages
```

On Windows, this approach is more challenging and not recommended - use Option 1 or 2 instead.

### Verifying Installation

You can verify your installation in the following ways:

1. Check for installed libraries:
   ```python
   import sys
   try:
       from opencog.atomspace import AtomSpace
       print("AtomSpace installed successfully")
   except ImportError:
       print("AtomSpace not installed correctly")
   ```

2. Run the included monitor script:
   ```bash
   python monitor_components.py
   ```

3. Check for running processes:
   ```bash
   ps aux | grep cogserver  # Linux/macOS
   tasklist | findstr cogserver  # Windows
   ```

## Using the Clay UI

The Clay UI is a powerful visualization tool for exploring the AtomSpace. After building OpenCog with Clay UI support:

### Starting the UI

```bash
# Navigate to the Clay UI build directory
cd clay-ui/build

# Launch the AtomSpace viewer
./opencog-atomspace-viewer
```

### Connecting to the AtomSpace

The Clay UI can connect to an AtomSpace in several ways:

1. **Direct Connection**: The viewer will automatically connect to a running AtomSpace in the same process.

2. **CogServer Connection**: Connect to a remote AtomSpace via CogServer:
   - Start CogServer: `cogserver -c ../lib/opencog.conf`
   - In Clay UI: File > Connect > Enter URL (typically ws://localhost:18080/opencog)

3. **File Loading**: Load an AtomSpace from a saved file:
   - In Clay UI: File > Open > Select saved AtomSpace file

### Navigation and Interaction

- **Left Mouse Button**: Rotate the view
- **Right Mouse Button**: Pan the view
- **Mouse Wheel**: Zoom in/out
- **Click on an Atom**: View details
- **Double-click on an Atom**: Focus on it and its connections
- **Search Box**: Find atoms by name or type
- **Filter Panel**: Filter by atom type or property
- **Layout Controls**: Adjust layout algorithm parameters

## Demo Scripts

This repository includes several demo scripts to get you started:

1. **atomspace_demo.py**: Creates a basic AtomSpace with concepts and relationships.
2. **pattern_matching_demo.py**: Demonstrates AtomSpace pattern matching capabilities.
3. **monitor_components.py**: Checks the status of installed OpenCog components.

Run these scripts to see OpenCog in action:

```bash
python atomspace_demo.py
python pattern_matching_demo.py
python monitor_components.py
```

## Component Health Monitor

The Component Health Monitor provides a web dashboard for monitoring OpenCog components:

```bash
# Start the health monitor
python ../scripts/component-health-monitor.py --web --port 8085

# Open a browser to http://localhost:8085
```

## Common Issues and Troubleshooting

### Python Bindings Not Found

```
Error: No module named 'opencog'
```

**Solution**:
- Check if the Python bindings are installed in the correct Python environment
- Try installing with `pip install --user` to install in the user's site-packages
- Check if `PYTHONPATH` includes the installation directory

### Missing Dependencies

```
error while loading shared libraries: libcogutil.so: cannot open shared object file
```

**Solution**:
- Run `ldconfig` to refresh the shared library cache
- Check if libraries are installed in standard locations (/usr/local/lib)
- Set `LD_LIBRARY_PATH` to include the directory with OpenCog libraries

### Clay UI Fails to Start

**Solution**:
- Check for OpenGL dependencies: `apt-get install libgl1-mesa-dev`
- Verify build output for errors
- Try running with debugging: `./opencog-atomspace-viewer --verbose`

### Performance Issues with Large AtomSpaces

**Solution**:
- Use filtering to limit displayed atoms
- Increase RAM allocated to the process
- Use the node collapsing feature to simplify visualization

## Additional Resources

- [OpenCog Documentation](https://wiki.opencog.org/)
- [AtomSpace Documentation](https://wiki.opencog.org/w/AtomSpace)
- [Clay UI Guide](https://github.com/opencog/clay-ui)
- [AtomSpace API Reference](https://wiki.opencog.org/w/AtomSpace_API)

## Contributing

We welcome contributions! To contribute:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

Please follow the coding guidelines in HACKING.md. 