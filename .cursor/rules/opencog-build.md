# OpenCog Build System

## Structure
- The OpenCog build system is a consolidated build system that builds all OpenCog components in the correct order.
- Components are built in a specific dependency order.
- CMake is used as the primary build system.

## Component Dependency Order
1. cogutil
2. atomspace
3. atomspace-storage, atomspace-rocks, atomspace-pgres
4. cogserver
5. learn
6. opencog
7. sensory
8. evidence

## Build Scripts
- `build.sh` - Bash script for Linux/macOS
- `build.ps1` - PowerShell script for Windows
- `install-dependencies.sh` - Script to install dependencies on Ubuntu/Debian
- `install-dependencies.ps1` - Script to install dependencies on Windows

## Repository Management
- `clone_repos.sh` - Script to clone all needed repositories on Linux/macOS
- `clone_repos.ps1` - Script to clone all needed repositories on Windows
- `repos.txt` - List of repositories to clone

## Clay UI
- The Clay UI system is integrated for visualization
- Located in the `clay-ui` directory
- Provides an interactive visualization of the AtomSpace

## Important Files
- `CMakeLists.txt` - Main CMake file
- `README.md` - Main documentation
- `TODO.md` - Current development tasks

## Coding Standards
- C++17 standard
- Follow the OpenCog coding style
- Document public APIs with Doxygen

## Testing
- Unit tests are in the `tests` directory of each component
- Run `make test` after building to run tests

## Documentation
- Main documentation is in the `README.md` file
- Additional documentation is in the component repositories
- API documentation is generated with Doxygen

## Windows-Specific Notes
- Use PowerShell scripts instead of Bash scripts
- Use vcpkg for dependencies
- Set execution policy to allow running scripts

## Linux-Specific Notes
- Use apt-get to install dependencies on Ubuntu/Debian
- Use yum to install dependencies on RHEL/CentOS
- Use pacman to install dependencies on Arch Linux 