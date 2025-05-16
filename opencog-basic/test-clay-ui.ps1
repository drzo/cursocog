#
# OpenCog Basic - Clay UI Test Script
# This script verifies the Clay UI integration and functionality
#

Write-Host "OpenCog Basic - Clay UI Test and Validation" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""

# Function to check for required dependencies
function Check-Dependencies {
    Write-Host "Checking for required dependencies..." -ForegroundColor Blue
    
    $missing_deps = $false
    $deps = @("cmake", "g++", "mingw32-make")
    
    foreach ($dep in $deps) {
        if (-not (Get-Command $dep -ErrorAction SilentlyContinue)) {
            Write-Host "Missing dependency: $dep" -ForegroundColor Red
            $missing_deps = $true
        }
    }
    
    # Check for OpenGL development libraries
    $glew_path = "C:\MinGW\include\GL\glew.h"
    if (-not (Test-Path $glew_path)) {
        Write-Host "OpenGL development libraries not found at expected location" -ForegroundColor Red
        $missing_deps = $true
    }
    
    if ($missing_deps) {
        Write-Host "Please install missing dependencies." -ForegroundColor Red
        Write-Host "For MinGW and CMake, use the install-dependencies.ps1 script." -ForegroundColor Yellow
        Write-Host "For OpenGL libraries, run: pacman -S mingw-w64-x86_64-glew" -ForegroundColor Yellow
        return $false
    } else {
        Write-Host "All dependencies are installed." -ForegroundColor Green
        return $true
    }
}

# Function to check directory structure
function Check-DirectoryStructure {
    Write-Host "Checking Clay UI directory structure..." -ForegroundColor Blue
    
    $missing = $false
    $directories = @("clay-ui", "clay-ui\include", "clay-ui\src", "clay-ui\examples", "clay-ui\cmake")
    
    foreach ($dir in $directories) {
        if (-not (Test-Path $dir -PathType Container)) {
            Write-Host "Missing directory: $dir" -ForegroundColor Red
            $missing = $true
        }
    }
    
    $files = @(
        "clay-ui\include\clay.h", 
        "clay-ui\include\opencog_renderer.h", 
        "clay-ui\include\opencog_atomspace_bridge.h", 
        "clay-ui\CMakeLists.txt"
    )
    
    foreach ($file in $files) {
        if (-not (Test-Path $file -PathType Leaf)) {
            Write-Host "Missing file: $file" -ForegroundColor Red
            $missing = $true
        }
    }
    
    if ($missing) {
        Write-Host "Clay UI directory structure is incomplete. Please run integrate-clay-ui.ps1 first." -ForegroundColor Red
        return $false
    } else {
        Write-Host "Clay UI directory structure is complete." -ForegroundColor Green
        return $true
    }
}

# Function to build the dashboard
function Build-Dashboard {
    Write-Host "Building dashboard..." -ForegroundColor Blue
    
    # Create build directory if it doesn't exist
    if (-not (Test-Path "clay-ui\build")) {
        New-Item -Path "clay-ui\build" -ItemType Directory | Out-Null
    }
    
    # Build
    Push-Location clay-ui\build
    
    # Run CMake
    if (-not (cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release)) {
        Write-Host "CMake configuration failed." -ForegroundColor Red
        Pop-Location
        return $false
    }
    
    # Run build
    if (-not (cmake --build . --target dashboard --config Release)) {
        Write-Host "Build failed." -ForegroundColor Red
        Pop-Location
        return $false
    }
    
    Pop-Location
    
    # Check if executable was created
    if (-not (Test-Path "clay-ui\build\dashboard.exe")) {
        Write-Host "Dashboard executable was not created." -ForegroundColor Red
        return $false
    }
    
    Write-Host "Dashboard built successfully." -ForegroundColor Green
    return $true
}

# Function to verify AtomSpace integration
function Verify-AtomSpaceIntegration {
    Write-Host "Verifying AtomSpace integration..." -ForegroundColor Blue
    
    $content = Get-Content "clay-ui\include\opencog_atomspace_bridge.h" -ErrorAction SilentlyContinue
    if (-not ($content -match "OpenCogAtomSpaceBridge_Initialize")) {
        Write-Host "AtomSpace bridge integration is missing." -ForegroundColor Red
        return $false
    }
    
    Write-Host "AtomSpace integration verified." -ForegroundColor Green
    return $true
}

# Function to test launcher script
function Test-LauncherScript {
    Write-Host "Testing dashboard launcher script..." -ForegroundColor Blue
    
    if (-not (Test-Path "opencog-dashboard.bat" -PathType Leaf)) {
        Write-Host "Launcher script 'opencog-dashboard.bat' is missing." -ForegroundColor Red
        return $false
    }
    
    Write-Host "Launcher script verified." -ForegroundColor Green
    return $true
}

# Function to generate test report
function Generate-Report {
    param (
        [bool]$dep_status,
        [bool]$dir_status,
        [bool]$build_status,
        [bool]$integration_status,
        [bool]$launcher_status
    )
    
    Write-Host ""
    Write-Host "====== Clay UI Test Report ======" -ForegroundColor Blue
    Write-Host ""
    Write-Host "Dependencies check: $(if ($dep_status) { "PASS" -f Green } else { "FAIL" -f Red })"
    Write-Host "Directory structure: $(if ($dir_status) { "PASS" -f Green } else { "FAIL" -f Red })"
    Write-Host "Build test: $(if ($build_status) { "PASS" -f Green } else { "FAIL" -f Red })"
    Write-Host "AtomSpace integration: $(if ($integration_status) { "PASS" -f Green } else { "FAIL" -f Red })"
    Write-Host "Launcher script: $(if ($launcher_status) { "PASS" -f Green } else { "FAIL" -f Red })"
    Write-Host ""
    
    # Calculate overall status
    $total_pass = @($dep_status, $dir_status, $build_status, $integration_status, $launcher_status) | Where-Object { $_ -eq $true } | Measure-Object | Select-Object -ExpandProperty Count
    
    if ($total_pass -eq 5) {
        Write-Host "All tests passed! Clay UI is properly integrated." -ForegroundColor Green
        Write-Host "You can run the dashboard with: .\opencog-dashboard.bat" -ForegroundColor Yellow
    } else {
        Write-Host "Some tests failed. Please address the issues above." -ForegroundColor Red
    }
}

# Main execution flow
$dep_status = Check-Dependencies
$dir_status = Check-DirectoryStructure
$build_status = Build-Dashboard
$integration_status = Verify-AtomSpaceIntegration
$launcher_status = Test-LauncherScript

# Generate test report
Generate-Report -dep_status $dep_status -dir_status $dir_status -build_status $build_status -integration_status $integration_status -launcher_status $launcher_status

# Return exit code based on test results
$exit_code = 0
if (-not ($dep_status -and $dir_status -and $build_status -and $integration_status -and $launcher_status)) {
    $exit_code = 1
}
exit $exit_code 