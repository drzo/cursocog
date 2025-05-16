# OpenCog Build System TODO

## Tasks
1. Scan subdirectories for README.md files and consolidate in root README.md
2. Scan subdirectories for CMakeLists.txt files and consolidate in root CMakeLists.txt
3. Scan subdirectories for .circleci/config.yml files and consolidate in root .circleci/config.yml
4. Scan config, cmake, readme files for prerequisites and dependencies
5. Download the dependency source files
6. Install and configure the necessary prerequisites and dependencies
7. Configure the build files and shell scripts in the root folder to build and install components:
   - 0. Prerequisites
   - 1. cogutil
   - 2. atomspace
   - 3. atomspace-storage, atomspace-rocks, atomspace-pgres
   - 4. cogserver
   - 5. learn
   - 6. opencog
   - 7. sensory
   - 8. evidence
   - 9. docker (where required)
   - 10. additional components listed in repos.txt
8. Implement Clay UI system for C++
9. Implement repository management scripts
10. Create comprehensive documentation for developers
11. Create package management scripts for multiple platforms

## Progress
- [x] Task 1: Consolidate README.md files - Created README-new.md with information from all components
- [x] Task 2: Consolidate CMakeLists.txt files - Created CMakeLists-new.txt with all components
- [x] Task 3: Consolidate .circleci/config.yml files - Created config-new.yml with all components
- [x] Task 4: Identify prerequisites and dependencies - Documented in installation scripts
- [x] Task 5: Download dependencies - Added in installation scripts
- [x] Task 6: Install and configure prerequisites - Created install-dependencies.sh and install-dependencies.ps1
- [x] Task 7: Create build scripts - Created build.sh and build.ps1
- [x] Task 8: Implement Clay UI system
  - [x] Created clay-ui directory structure
  - [x] Added download scripts (download.sh, download.ps1)
  - [x] Created CMakeLists.txt for Clay UI
  - [x] Implemented OpenCog renderer with OpenGL support
  - [x] Created OpenCog bindings for Clay UI
  - [x] Added example applications (atomspace_viewer, dashboard)
  - [x] Advanced AtomSpace visualization with physics simulation
  - [x] Integration with AtomSpace implementations
- [x] Task 9: Implement repository management
  - [x] Created repos.txt listing all required components
  - [x] Implemented clone_repos.sh for Linux/macOS
  - [x] Implemented clone_repos.ps1 for Windows
- [x] Task 10: Create developer documentation
  - [x] Added .cursor/rules/opencog-build.md with build system information
  - [x] Created detailed progress reports in wiki/chat/
  - [x] Documented component dependencies and build order
- [x] Task 11: Create package management scripts
  - [x] Created Linux package creation script (create-packages.sh)
  - [x] Created Windows installer script (create-windows-installer.ps1)
  - [x] Created Chocolatey package creation script (create-chocolatey-package.ps1)
  - [x] Added packaging documentation (CHOCO.md)

## Next Steps

### Top Priorities for Completing OpenCog Basic
1. Test and Finalize Clay UI Integration
   - [x] Created test scripts for Windows and Linux
   - [x] Added dashboard launcher scripts
   - [x] Created integration scripts
   - [x] Created test scripts to validate the integration
   - [x] Complete any remaining UI views (AtomSpace, Examples, Settings)

2. Create AtomSpace Examples for Beginners
   - [x] Created sample AtomSpace data files
   - [x] Added example scripts to populate RocksDB with sample data
   - [x] Created tutorial materials accessible from the dashboard
   - [x] Added well-commented examples showing common AtomSpace operations
   - [x] Included diverse examples (pattern matching, reasoning, persistence)

3. Complete Dependency Management
   - [x] Integrated CogInit
   - [x] Added verification tools in test scripts
   - [x] Added missing library checks to install-dependencies scripts
   - [x] Created build-with-coginit.sh/ps1 scripts
   - [ ] Create a standalone dependency verification tool

4. Create Installation Packages
   - [x] Created Linux package creation script (DEB/RPM)
   - [x] Created Windows installer script (NSIS)
   - [x] Created Chocolatey package script for Windows
   - [x] Added package creation documentation to README
   - [ ] Create macOS package (DMG)
   - [ ] Add automated package building to CI

5. Multi-Platform Testing
   - [x] Created test scripts for Linux and Windows
   - [x] Added platform-specific checks in test scripts
   - [x] Updated documentation with platform requirements
   - [ ] Create comprehensive test matrix for all platforms
   - [ ] Add automated multi-platform testing to CI

### Additional Future Tasks
1. Complete Clay UI integration
   - [ ] Finalize real-time visualization of AtomSpace changes
   - [ ] Add advanced interaction features (node/edge editing, filtering, search)
   - [ ] Add custom visualization modes (hierarchical, radial, force-directed)
   - [ ] Optimize rendering performance for large AtomSpace graphs

2. Enhance build system
   - [ ] Add incremental build support to speed up development
   - [ ] Optimize parallel builds with dependency tracking
   - [ ] Add component-specific configuration options
   - [ ] Create pre-configured build profiles for different use cases

3. Implement additional tools
   - [ ] Create visual build system dashboard with build status
   - [ ] Add dependency graph visualization for components
   - [ ] Create component health monitoring with metrics
   - [ ] Implement integration test framework

4. Enhance package management
   - [ ] Create Snap package for Linux distributions
   - [ ] Implement package version management system
   - [ ] Create auto-update mechanism for installed packages
   - [ ] Add dependency resolution in package scripts

## Recent Updates
- Created Chocolatey package creation script (create-chocolatey-package.ps1)
- Added CHOCO.md documentation for creating Chocolatey packages
- Created Clay UI test and integration scripts (test-clay-ui.sh, test-clay-ui.ps1)
- Created package creation scripts (create-packages.sh, create-windows-installer.ps1)
- Added dashboard application and launcher scripts (opencog-dashboard.sh, opencog-dashboard.bat)
- Updated README with new features and installation options
- Updated Clay UI CMakeLists.txt to include dashboard target
- Created comprehensive testing scripts for validating Clay UI integration
- Added AtomSpace examples for beginners (basic concepts, persistence, pattern matching)
- Created installation packages for Linux (DEB/RPM) and Windows (NSIS)

See wiki/chat/2024_07_18_00_00_OpenCog_Build_Progress.md for a detailed summary of all completed work. 