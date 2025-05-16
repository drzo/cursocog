# AtomSpace Examples

This directory contains example scripts and data to help newcomers understand and use the AtomSpace effectively. These examples are designed to be used with the `opencog-basic` build and provide practical demonstrations of key AtomSpace concepts.

## Example Files

### 1. Basic AtomSpace Examples (`basic_atomspace.scm`)

This file demonstrates fundamental AtomSpace concepts including:
- Creating basic Atoms (Nodes and Links)
- Working with TruthValues
- Basic querying with the `Get` link
- Pattern matching
- Executable links
- Creating a simple knowledge base

**Usage:**
```scheme
(load "basic_atomspace.scm")
(cog-execute! find-fruits)  # Example query
```

### 2. RocksDB Persistence (`populate_rocksdb.scm`)

This example shows how to use RocksDB for AtomSpace persistence, including:
- Setting up a RocksDB connection
- Storing Atoms in the database
- Verifying that data was stored correctly
- Loading data from the database
- Querying the stored data

**Usage:**
```scheme
guile -l populate_rocksdb.scm
```

### 3. Pattern Matching (`pattern_matching.scm`)

An advanced tutorial on pattern matching capabilities with examples of:
- Creating a knowledge base about people, relationships, and properties
- Basic pattern matching
- Advanced pattern matching with conjunctions
- Matching with variables in multiple positions
- Numerical constraints in patterns
- Absence patterns (finding things that don't match certain criteria)
- Exercise patterns for practice

**Usage:**
```scheme
guile -l pattern_matching.scm
```

## Requirements

To run these examples, you need:

1. A built version of `opencog-basic` with its dependencies
2. Guile Scheme interpreter
3. For persistence examples, the RocksDB backend must be built and available

## Getting Started

1. Build the `opencog-basic` project using the provided build scripts
2. Launch the Guile interpreter in this directory or use the OpenCog CogServer with these examples
3. Start with `basic_atomspace.scm` to learn the fundamentals
4. Move on to `pattern_matching.scm` to learn more advanced querying techniques
5. Try `populate_rocksdb.scm` to understand persistence

## Integration with Dashboard

These examples are also accessible through the OpenCog Dashboard, which provides a visual interface for exploring and understanding AtomSpace operations. To use the dashboard:

1. Launch the OpenCog Dashboard with:
   ```
   # Windows
   opencog-dashboard.bat
   
   # Linux
   ./opencog-dashboard.sh
   ```

2. Navigate to the Tutorials section to see guided versions of these examples with interactive elements

## Additional Resources

For more information on AtomSpace and OpenCog:

1. [OpenCog Wiki](https://wiki.opencog.org/)
2. [AtomSpace Documentation](https://wiki.opencog.org/w/AtomSpace)
3. [Pattern Matcher Documentation](https://wiki.opencog.org/w/Pattern_Matcher)

## Exercise Solutions

The pattern matching examples include exercises. If you're stuck, solutions are included in the `pattern_matching.scm` file as `friends-of-friends` and `same-profession-different-cities`. 