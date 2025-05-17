# PowerShell script to set up OpenCog using Docker
# This script automates the setup of Docker containers for OpenCog development

Write-Host "OpenCog Docker Setup Script" -ForegroundColor Cyan
Write-Host "==========================" -ForegroundColor Cyan

# Check if Docker is installed
$dockerCheck = docker --version 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Docker is not installed. Please install Docker Desktop for Windows first:" -ForegroundColor Red
    Write-Host "https://www.docker.com/products/docker-desktop/" -ForegroundColor Yellow
    exit
}

# Current directory
$currentDir = (Get-Location).Path
Write-Host "Current directory: $currentDir" -ForegroundColor Gray

# Set up OpenCog using Docker
Write-Host "Setting up OpenCog Docker environment..." -ForegroundColor Yellow

# Pull the OpenCog Docker image
Write-Host "Pulling OpenCog Docker image..." -ForegroundColor Yellow
docker pull opencog/opencog-dev

# Create a Docker Compose file
Write-Host "Creating Docker Compose configuration..." -ForegroundColor Yellow
@"
version: '3'
services:
  opencog:
    image: opencog/opencog-dev
    container_name: opencog-dev
    volumes:
      - ${currentDir}:/opencog
    ports:
      - "17001:17001"  # For CogServer network API
    working_dir: /opencog
    command: bash
    stdin_open: true   # docker run -i
    tty: true          # docker run -t

  relex:
    image: opencog/relex
    container_name: opencog-relex
    command: /bin/sh -c "./opencog-server.sh"
"@ | Out-File -FilePath "docker-compose.yml" -Encoding utf8

# Create a helper script to run commands in the Docker container
@"
#!/bin/bash
# Helper script to run commands inside the OpenCog Docker container

# Ensure Docker container is running
CONTAINER_RUNNING=\$(docker ps -q -f name=opencog-dev)
if [ -z "\$CONTAINER_RUNNING" ]; then
    # Container not running, start it
    echo "Starting OpenCog Docker container..."
    docker-compose up -d opencog
fi

# Execute the command in the container
docker exec -it opencog-dev \$@
"@ | Out-File -FilePath "opencog-docker.sh" -Encoding utf8 -NoNewline

# Create Windows batch wrapper for the helper script
@"
@echo off
wsl bash opencog-docker.sh %*
"@ | Out-File -FilePath "opencog-docker.bat" -Encoding utf8 -NoNewline

# Create a README with instructions
@"
# OpenCog Docker Environment

This directory contains scripts for running OpenCog in Docker containers.

## Quick Start

1. Make sure Docker Desktop is running.

2. Start the OpenCog environment:
   ```
   docker-compose up -d
   ```

3. Connect to the running container:
   ```
   docker exec -it opencog-dev bash
   ```
   
   Or use the helper script:
   ```
   .\opencog-docker.bat bash
   ```

4. Inside the container, you can build and run OpenCog components:
   ```
   cd /opencog/cogutil
   mkdir build
   cd build
   cmake ..
   make -j
   make install
   ```

5. To stop the environment:
   ```
   docker-compose down
   ```

## Directory Structure

The current directory is mounted as `/opencog` in the Docker container.
Any changes you make to files in this directory will be reflected in the container.

## Relex (Link Grammar Server)

The Relex container is included for natural language processing functionality.
It runs a server that OpenCog can connect to for parsing natural language.
"@ | Out-File -FilePath "DOCKER-README.md" -Encoding utf8

Write-Host "`nSetup complete!" -ForegroundColor Green
Write-Host "To use the OpenCog Docker environment:" -ForegroundColor Cyan
Write-Host "1. Start Docker Desktop" -ForegroundColor White
Write-Host "2. Start the containers: docker-compose up -d" -ForegroundColor White
Write-Host "3. Connect to the container: docker exec -it opencog-dev bash" -ForegroundColor White
Write-Host "   Or use the helper script: .\opencog-docker.bat bash" -ForegroundColor White
Write-Host "`nSee DOCKER-README.md for more information." -ForegroundColor Yellow 