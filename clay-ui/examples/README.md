# Clay UI Examples for OpenCog

This directory contains example applications demonstrating how to use Clay UI with OpenCog.

## Examples

1. **simple_window.c** - A basic example showing how to create a window with Clay UI and display a simple AtomSpace viewer interface.

## Building Examples

The examples are built automatically when you build the Clay UI system. You can run the simple window example after building with:

```bash
# On Linux/Unix
./build/clay-ui-example

# On Windows
.\build\Debug\clay-ui-example.exe
```

## Creating New Examples

To create a new example:

1. Create a new C or C++ file in this directory
2. Add it to the `clay-ui/CMakeLists.txt` file
3. Build the project

## Requirements

All examples require:
- OpenGL
- GLEW
- X11 (on Linux) or Win32 API (on Windows)

## Integration with OpenCog

These examples are designed to show how Clay UI can be used to create user interfaces for OpenCog applications. They demonstrate various aspects of the OpenCog system, including:

- AtomSpace visualization
- Pattern Matcher interfaces
- Rule Engine visualization
- PLN inference visualization 