# Clay UI for OpenCog

This directory contains the integration of the Clay UI library into OpenCog. Clay is a high-performance UI layout library in C, designed to provide a modern UI experience with minimal dependencies.

## Features

- Microsecond layout performance
- Flexbox-like layout model for complex, responsive layouts
- Single ~4k LOC header file with zero dependencies
- Static arena-based memory use with low total memory overhead
- React-like nested declarative syntax
- Renderer agnostic: outputs a sorted list of rendering primitives

## Implementation Plan

1. **Integration**: Integrate Clay UI library (clay.h) into the OpenCog build system
2. **Renderer**: Implement a renderer for OpenCog that works with the Clay rendering commands
3. **Components**: Create reusable OpenCog-specific UI components
4. **Theming**: Implement an OpenCog theme with consistent colors and styling
5. **Examples**: Provide example applications demonstrating integration with AtomSpace visualization

## Dependencies

- C/C++ compiler with C99 support
- OpenCog core libraries

## Usage

See the `examples` directory for sample applications demonstrating how to use Clay UI with OpenCog.

## License

Clay UI is distributed under the zlib license. See the LICENSE file for details. 