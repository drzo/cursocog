# Building Boost on Windows

This guide explains how to build Boost from source on Windows for OpenCog development.

## Prerequisites

- Visual Studio 2019 or later with C++ tools
- 7zip or similar archive extraction tool
- PowerShell 5.0 or later

## Building Steps

1. Extract the Boost archive from ../deps/boost_1_79_0.zip to a directory on your system

2. Open a Visual Studio Developer Command Prompt

3. Navigate to the Boost directory:
   `
   cd C:\path\to\boost_1_79_0
   `

4. Run bootstrap:
   `
   bootstrap.bat
   `

5. Build Boost with the required libraries:
   `
   .\b2 --with-system --with-filesystem --with-thread --with-program_options --with-regex --address-model=64 -j4 toolset=msvc
   `

   Note: -j4 specifies using 4 cores for compilation. Adjust according to your system.

6. Set the BOOST_ROOT environment variable:
   `
   setx BOOST_ROOT C:\path\to\boost_1_79_0
   `

7. Add the library path to your system PATH:
   `
   setx PATH "%PATH%;C:\path\to\boost_1_79_0\stage\lib"
   `

## Verification

To verify your Boost installation:

1. Open a new command prompt (to refresh environment variables)

2. Run:
   `
   echo %BOOST_ROOT%
   `
   
   This should output the path to your Boost installation.

3. Check if the libraries exist:
   `
   dir %BOOST_ROOT%\stage\lib
   `

   You should see various .lib and .dll files.
