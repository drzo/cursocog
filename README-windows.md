# Building OpenCog on Windows

This guide explains the options for building and running OpenCog on Windows.

## Recommended Options

OpenCog is primarily designed for Unix/Linux environments. For Windows users, we recommend the following approaches, in order of preference:

### 1. Windows Subsystem for Linux (WSL) - RECOMMENDED

The most reliable way to build OpenCog on Windows is to use WSL:

```powershell
# Run in PowerShell
.\build.ps1 -WSL
```

This will:
1. Install WSL and Ubuntu if not already present
2. Set up the build environment in WSL
3. Create a build script to compile OpenCog components in the correct order

Benefits:
- Full Linux compatibility
- Native performance
- Access files from both Windows and Linux
- Can run GUI applications with WSLg

### 2. Docker Containers

Another good option is to use Docker containers:

```powershell
# Run in PowerShell
.\build.ps1 -Docker
```

This will:
1. Check for Docker installation
2. Pull the necessary OpenCog Docker images
3. Set up a Docker Compose configuration and helper scripts

Benefits:
- Isolated environment
- Pre-configured with all dependencies
- Can be easily reset to a clean state
- Works on all platforms (Windows, macOS, Linux)

### 3. MSYS2/MinGW Environment

For those who prefer a more native-like experience but still need Unix compatibility:

1. Install MSYS2 from https://www.msys2.org/
2. Open MSYS2 MinGW64 shell
3. Install dependencies:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-boost mingw-w64-x86_64-cmake git make
   ```
4. Clone and build OpenCog repositories in sequence

Benefits:
- More integrated with Windows
- Uses MinGW which has better Unix compatibility than MSVC
- Can use Windows paths and tools

## Native Windows Build (CHALLENGING)

Building OpenCog natively on Windows with MSVC is extremely challenging due to numerous compatibility issues:

```powershell
# Install dependencies only (not recommended to continue with actual build)
.\build.ps1 -InstallDependencies
```

Challenges include:
- Socket/winsock header conflicts
- Multiple redefinitions of system types
- Conflicts with standard C/C++ functions
- Missing Unix headers
- Complex dependencies with poor Windows support
- Incompatible function linkage

**We do not recommend pursuing a native Windows build** unless you are prepared for significant code modifications and troubleshooting.

## Build Sequence

Regardless of the method you choose, OpenCog components should be built in this order:

1. cogutil
2. atomspace
3. atomspace-storage
4. atomspace-rocks
5. atomspace-pgres
6. cogserver
7. opencog

For WSL and Docker options, scripts are provided to automate this process.

## Getting Help

For more information or help with building OpenCog on Windows:

1. OpenCog GitHub: https://github.com/opencog
2. OpenCog Wiki: https://wiki.opencog.org
3. OpenCog Discord: https://discord.gg/JEK8EpDxkJ 