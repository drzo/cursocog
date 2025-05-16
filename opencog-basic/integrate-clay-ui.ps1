# OpenCog Basic - Clay UI Integration Wrapper
# This script integrates and builds the Clay UI dashboard

Write-Host "OpenCog Basic - Clay UI Integration and Build" -ForegroundColor Cyan
Write-Host "===========================================" -ForegroundColor Cyan
Write-Host ""

# Run the integration script
if (Test-Path ".\clay-ui-integration.ps1") {
    Write-Host "Running Clay UI integration script..." -ForegroundColor Yellow
    & .\clay-ui-integration.ps1
} else {
    Write-Host "ERROR: clay-ui-integration.ps1 not found." -ForegroundColor Red
    exit 1
}

# Create build directory
if (-not (Test-Path ".\clay-ui\build")) {
    Write-Host "Creating Clay UI build directory..." -ForegroundColor Yellow
    New-Item -Path ".\clay-ui\build" -ItemType Directory | Out-Null
}

# Build the dashboard
Write-Host "Building OpenCog Dashboard..." -ForegroundColor Yellow
Push-Location ".\clay-ui\build"

# Run CMake
Write-Host "Running CMake..." -ForegroundColor Yellow
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
Write-Host "Compiling dashboard..." -ForegroundColor Yellow
cmake --build . --target dashboard --config Release

Pop-Location

# Check if build succeeded
if (Test-Path ".\clay-ui\build\dashboard.exe") {
    Write-Host "Dashboard built successfully!" -ForegroundColor Green
    Write-Host ""
    Write-Host "You can now run the dashboard with: .\opencog-dashboard.bat" -ForegroundColor Yellow
} else {
    Write-Host "Dashboard build failed. Please check the error messages above." -ForegroundColor Red
} 