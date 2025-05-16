# OpenCog Basic Components Integration

This project provides scripts to integrate the essential core components of OpenCog into a standalone repository. The goal is to create a streamlined, efficient setup that includes only the necessary components for basic OpenCog functionality.

## Overview

The OpenCog project consists of many components, but not all are needed for basic operation. This integration focuses on the core components:

1. **cogutil** - Utility library with common functions and data structures
2. **atomspace** - Core knowledge representation framework
3. **atomspace-storage** - Persistence API for AtomSpace
4. **atomspace-rocks** - RocksDB backend for AtomSpace persistence
5. **cogserver** - Network server providing AtomSpace API access

## Getting Started

### Prerequisites

- Git
- CMake 3.12+
- C++17 compatible compiler
- Boost 1.68+
- Python 3.6+ with development headers

### Setup

1. Clone the basic components:

#### On Windows:
```powershell
.\clone-basic.ps1
```

#### On Linux/macOS:
```bash
./clone-basic.sh
```

This will create an `opencog-basic` directory containing all the essential components configured to build together.

### Directory Structure

After running the clone script, you'll have a directory structure like:
```
opencog-basic/
  ├── cogutil/
  ├── atomspace/
  ├── atomspace-storage/
  ├── atomspace-rocks/
  ├── cogserver/
  ├── CMakeLists.txt
  ├── README.md
  ├── build.sh
  ├── build.ps1
  └── install-dependencies.sh/ps1
```

### Building

1. Install dependencies:

#### On Windows:
```powershell
cd opencog-basic
.\install-dependencies.ps1
```

#### On Linux/macOS:
```bash
cd opencog-basic
./install-dependencies.sh
```

2. Build all components:

#### On Windows:
```powershell
.\build.ps1
```

#### On Linux/macOS:
```bash
./build.sh
```

## Using the Basic Components

After building, you can use the components individually or together as a cohesive system.

### Starting a CogServer

```bash
# Linux/macOS
./opencog-basic/cogserver/build/opencog/cogserver/server/cogserver

# Windows
.\opencog-basic\cogserver\build\opencog\cogserver\server\cogserver.exe
```

### Using AtomSpace with RocksDB Storage

```python
from opencog.atomspace import AtomSpace, types
from opencog.type_constructors import *
from opencog.persist.api import StorageNode

# Create an AtomSpace
atomspace = AtomSpace()
set_type_ctor_atomspace(atomspace)

# Connect to RocksDB storage
storage = StorageNode("rocks:///tmp/atomspace-data")
storage.open()
atomspace.set_storage(storage)

# Create some atoms
concept = ConceptNode("example")
number = NumberNode("42")
evaluation = EvaluationLink(
    PredicateNode("has_value"),
    ListLink(concept, number)
)

# Store atoms
atomspace.store_atom(evaluation)

# Close storage
storage.close()
```

## Adding Advanced Components

If you need additional functionality, you can uncomment the advanced components in `repos-basic.txt` and run the clone script again. These include:

- **atomspace-pgres** - PostgreSQL backend for AtomSpace persistence
- **unify** - Pattern matching and unification
- **ure** - Rule engine for inference

## Contributing

Contributions to improve the integration are welcome. Please feel free to submit pull requests or open issues for any improvements or bug fixes. 