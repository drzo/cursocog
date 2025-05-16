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
  - [x] Added example application
  - [x] Advanced AtomSpace visualization with physics simulation
  - [ ] Integrate with real AtomSpace implementations
- [x] Task 9: Implement repository management
  - [x] Created repos.txt listing all required components
  - [x] Implemented clone_repos.sh for Linux/macOS
  - [x] Implemented clone_repos.ps1 for Windows
- [x] Task 10: Create developer documentation
  - [x] Added .cursor/rules/opencog-build.md with build system information
  - [x] Created detailed progress reports in wiki/chat/
  - [x] Documented component dependencies and build order

## Next Steps

### Top Priorities for Completing OpenCog Basic
1. Test and Finalize Clay UI Integration
   - [ ] Test integration on Windows and Linux
   - [ ] Verify dashboard launches correctly
   - [ ] Ensure visualization connects to AtomSpace instances
   - [ ] Fix any bugs in the Clay UI build system
   - [ ] Test with real-world AtomSpace data

2. Create AtomSpace Examples for Beginners
   - [x] Create sample AtomSpace data files
   - [x] Add example scripts to populate RocksDB with sample data
   - [x] Create tutorial materials accessible from the dashboard
   - [x] Add well-commented examples showing common AtomSpace operations
   - [x] Include diverse examples (pattern matching, reasoning, persistence)

3. Complete Dependency Management
   - [ ] Finish CogInit integration testing
   - [ ] Verify all dependencies are detected and installed properly
   - [ ] Add missing library checks to install-dependencies scripts
   - [ ] Test dependency resolver on clean machines
   - [ ] Create a dependency verification tool

4. Improve Documentation and Create Packages
   - [x] Create user documentation for the dashboard and visualizations
   - [x] Add screenshots and usage examples to README.md
   - [x] Document all supported storage backends with example commands
   - [ ] Create Windows installer (NSIS or WiX)
   - [ ] Create DEB/RPM packages for Linux distributions

5. Multi-Platform Testing
   - [ ] Test on Ubuntu Linux 20.04 and 22.04
   - [ ] Test on Windows 10/11 with both MSVC and MinGW toolchains
   - [ ] Fix platform-specific issues
   - [ ] Document platform-specific setup procedures
   - [ ] Create automated test scripts for each platform

### Additional Future Tasks
1. Complete Clay UI integration
   - [ ] Connect with live AtomSpace instances via StorageNode API
   - [ ] Add advanced interaction features (node/edge editing, filtering, search)
   - [ ] Implement real-time visualization of AtomSpace changes
   - [ ] Add custom visualization modes (hierarchical, radial, force-directed)
   - [ ] Optimize rendering performance for large AtomSpace graphs
   - [ ] Create user documentation for Clay UI

2. Enhance build system
   - [ ] Add incremental build support to speed up development
   - [ ] Optimize parallel builds with dependency tracking
   - [ ] Add component-specific configuration options
   - [ ] Create pre-configured build profiles for different use cases
   - [ ] Implement automated dependency resolution
   - [ ] Add support for cross-compilation

3. Implement additional tools
   - [ ] Create visual build system dashboard with build status
   - [ ] Add dependency graph visualization for components
   - [ ] Create component health monitoring with metrics
   - [ ] Implement integration test framework
   - [ ] Add continuous profiling and performance tracking
   - [ ] Create development environment setup wizard

## Recent Updates
- Added advanced AtomSpace visualization with force-directed graph layout
- Created repository management scripts (clone_repos.sh, clone_repos.ps1)
- Added comprehensive developer documentation in .cursor/rules/opencog-build.md
- Created detailed progress report in wiki/chat/2024_07_18_00_00_OpenCog_Build_Progress.md
- Added Clay UI integration with dashboard for enhanced user experience
- Updated top priorities to focus on completing OpenCog Basic
- Created comprehensive AtomSpace examples for beginners (basic concepts, persistence, pattern matching)
- Updated README.md with information about examples and usage instructions

See wiki/chat/2024_07_18_00_00_OpenCog_Build_Progress.md for a detailed summary of all completed work. 