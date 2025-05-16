cmake_minimum_required(VERSION 3.10)

project(clay-ui VERSION 0.1.0 LANGUAGES C CXX)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Check if OpenGL is available
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Main library
add_library(clay-ui INTERFACE)
target_include_directories(clay-ui INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# If we're on Windows, we need to use the Win32 API for windowing
if(WIN32)
    target_compile_definitions(clay-ui INTERFACE CLAY_WINDOWS)
endif()

# If we're on Linux/Unix, we need to use X11 or Wayland for windowing
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(clay-ui INTERFACE ${X11_LIBRARIES})
    target_include_directories(clay-ui INTERFACE ${X11_INCLUDE_DIR})
    target_compile_definitions(clay-ui INTERFACE CLAY_LINUX)
endif()

# Add example renderer with OpenGL
add_library(clay-ui-renderer INTERFACE)
target_include_directories(clay-ui-renderer INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderers
)
target_link_libraries(clay-ui-renderer INTERFACE
    clay-ui
    OpenGL::GL
    GLEW::GLEW
)

# Add AtomSpace integration library
add_library(clay-ui-atomspace
    src/opencog_atomspace_bridge.cpp
    src/opencog_clay_bindings.cpp
    src/atom_visualizer.cpp
)
target_include_directories(clay-ui-atomspace PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(clay-ui-atomspace
    clay-ui
    clay-ui-renderer
)

# Add example executables
add_executable(clay-ui-example 
    examples/simple_window.c
)
target_link_libraries(clay-ui-example
    clay-ui
    clay-ui-renderer
)

add_executable(atomspace-viewer
    examples/atomspace_viewer.cpp
)
target_link_libraries(atomspace-viewer
    clay-ui
    clay-ui-renderer
    clay-ui-atomspace
)

# Add dashboard application
add_executable(dashboard
    examples/dashboard/dashboard.cpp
)
target_link_libraries(dashboard
    clay-ui
    clay-ui-renderer
    clay-ui-atomspace
)

# Add dashboard launcher
add_executable(dashboard-launcher
    examples/dashboard_launcher.cpp
)
target_link_libraries(dashboard-launcher
    clay-ui-atomspace
)

# Installation
install(TARGETS clay-ui clay-ui-renderer clay-ui-atomspace dashboard atomspace-viewer dashboard-launcher
    EXPORT clay-ui-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/ 
    DESTINATION include/clay
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY include/
    DESTINATION include/clay
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT clay-ui-targets
    FILE clay-ui-targets.cmake
    DESTINATION lib/cmake/clay-ui
)

# Create the configuration files
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/clay-ui-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/clay-ui-config.cmake
    INSTALL_DESTINATION lib/cmake/clay-ui
)

# Install the config files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/clay-ui-config.cmake
    DESTINATION lib/cmake/clay-ui
) 