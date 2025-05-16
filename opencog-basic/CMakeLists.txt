# OpenCog Basic Components
# CMake build file for minimal OpenCog setup

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(opencog-basic)

# Default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

# Add each component subdirectory
ADD_SUBDIRECTORY(cogutil)
ADD_SUBDIRECTORY(atomspace)
ADD_SUBDIRECTORY(atomspace-storage)
ADD_SUBDIRECTORY(atomspace-rocks)
ADD_SUBDIRECTORY(cogserver)

MESSAGE(STATUS "OpenCog basic components build configured")
