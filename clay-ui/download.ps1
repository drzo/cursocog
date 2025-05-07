# PowerShell script to download Clay UI library

# Create directories
Write-Host "Downloading Clay UI library..."
New-Item -ItemType Directory -Path "src" -Force | Out-Null
Set-Location -Path "src"

# Download clay.h
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/nicbarker/clay/main/clay.h" -OutFile "clay.h"
Write-Host "Downloaded Clay UI library to src/clay.h"

# Download an example renderer
New-Item -ItemType Directory -Path "renderers" -Force | Out-Null
Set-Location -Path "renderers"
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/nicbarker/clay/main/renderers/opengl/gl_renderer.h" -OutFile "gl_renderer.h"
Write-Host "Downloaded OpenGL renderer to src/renderers/gl_renderer.h"

# Return to the root directory
Set-Location -Path "../.."
Write-Host "Clay UI download complete!" 