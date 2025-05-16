# OpenCog: Artificial General Intelligence Framework

[![CircleCI](https://circleci.com/gh/opencog/opencog.svg?style=svg)](https://circleci.com/gh/opencog/opencog)

## Overview

OpenCog is an open-source framework for artificial general intelligence (AGI). It provides a flexible and extensible platform for developing and integrating various AI components and algorithms.

This repository contains the consolidated build system for the entire OpenCog ecosystem, bringing together multiple components into a unified framework. It aims to simplify the process of building and deploying OpenCog by providing scripts and tools that manage dependencies and compilation across all components.

## Architecture

The OpenCog framework is composed of several core components:

1. **CogUtil** - Common OpenCog C++ utilities
2. **AtomSpace** - Hypergraph database and query engine
3. **AtomSpace Storage Backends**:
   - atomspace-storage - Base storage API
   - atomspace-rocks - RocksDB storage backend
   - atomspace-pgres - PostgreSQL storage backend
4. **CogServer** - Network server for the AtomSpace
5. **Learn** - Language learning subsystem
6. **OpenCog Core** - Framework integrating NLP, reasoning, and other components
7. **Sensory** - Sensorimotor interfacing system
8. **Evidence** - Evidence-based reasoning system

## Features

- **Hypergraph Database** - AtomSpace provides a sophisticated in-RAM hypergraph database for knowledge representation
- **Reasoning** - Multiple reasoning systems including PLN (Probabilistic Logic Networks)
- **Natural Language Processing** - Tools for language parsing, generation, and learning
- **Sensorimotor Systems** - Interfaces for perception and action
- **Network Distribution** - Distributed AtomSpace capabilities via CogServer

## Prerequisites

To build and run OpenCog, you'll need:

### Ubuntu/Debian

```bash
sudo apt-get install build-essential cmake
sudo apt-get install libboost-dev libboost-filesystem-dev libboost-program-options-dev
sudo apt-get install libboost-system-dev libboost-thread-dev
sudo apt-get install guile-3.0-dev
sudo apt-get install cxxtest
sudo apt-get install libpq-dev librocksdb-dev
sudo apt-get install binutils-dev libiberty-dev
```

### Windows

- Visual Studio with C++ development tools
- CMake
- Git
- Boost libraries (through vcpkg)
- Guile (optional)

## Dependency Management

## Dependency Verification

Before building OpenCog, you can verify that all required dependencies are installed correctly using the dependency verification tool:

### Linux/macOS
```bash
./scripts/verify-dependencies.sh
```

### Windows
```powershell
.\scripts\verify-dependencies.ps1
```

The tool will check for all required dependencies, verify their versions, and provide installation instructions for any missing or outdated components.

## Dependency Installation

If dependencies are missing, you can install them using the provided scripts:

### Linux
```bash
sudo ./install-dependencies.sh
```

### Windows
```powershell
# Run as Administrator
.\install-dependencies.ps1
```

### macOS
```bash
# Install using Homebrew
brew install cmake boost cxxtest guile python3 rocksdb postgresql
pip3 install cython pytest numpy scipy
```

## Building OpenCog

### Using the Automated Build Scripts

#### Linux/macOS
```bash
./build.sh
```

For specific components:
```bash
./build.sh --component cogutil
```

#### Windows
```powershell
.\build.ps1
```

For specific components:
```powershell
.\build.ps1 -Component cogutil
```

### Manual Building

To build manually, follow this order:

1. CogUtil
2. AtomSpace
3. Unify
4. URE
5. AtomSpace-Storage
6. AtomSpace-Rocks/AtomSpace-Pgres
7. CogServer
8. Attention
9. Spacetime
10. PLN
11. LG-Atomese
12. Learn
13. OpenCog
14. Sensory
15. Evidence

For each component, execute:
```bash
mkdir build
cd build
cmake ..
make -j
sudo make install
```

## Development

OpenCog is an actively developed open-source project. Contributions are welcome!

### Getting Started

1. Fork the repository
2. Pick an issue or feature to work on
3. Create a branch
4. Make your changes
5. Submit a pull request

### Documentation

- [OpenCog Wiki](https://wiki.opencog.org)
- [AtomSpace Documentation](https://github.com/opencog/atomspace)
- [Language Learning](https://github.com/opencog/learn)

## Component Repositories

- [opencog/cogutil](https://github.com/opencog/cogutil)
- [opencog/atomspace](https://github.com/opencog/atomspace)
- [opencog/atomspace-storage](https://github.com/opencog/atomspace-storage)
- [opencog/atomspace-rocks](https://github.com/opencog/atomspace-rocks)
- [opencog/atomspace-pgres](https://github.com/opencog/atomspace-pgres)
- [opencog/unify](https://github.com/opencog/unify)
- [opencog/ure](https://github.com/opencog/ure)
- [opencog/cogserver](https://github.com/opencog/cogserver)
- [opencog/attention](https://github.com/opencog/attention)
- [opencog/spacetime](https://github.com/opencog/spacetime)
- [opencog/pln](https://github.com/opencog/pln)
- [opencog/lg-atomese](https://github.com/opencog/lg-atomese)
- [opencog/learn](https://github.com/opencog/learn)
- [opencog/opencog](https://github.com/opencog/opencog)
- [opencog/sensory](https://github.com/opencog/sensory)
- [opencog/evidence](https://github.com/opencog/evidence)

## License

OpenCog is licensed under the AGPL-3.0 license. See the [LICENSE](LICENSE) file for details.
