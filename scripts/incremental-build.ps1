# OpenCog Incremental Build Script for Windows
# This script builds only the components that have changed since the last build

# Define colors for output
$Green = [System.ConsoleColor]::Green
$Red = [System.ConsoleColor]::Red
$Yellow = [System.ConsoleColor]::Yellow
$Blue = [System.ConsoleColor]::Blue

function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    else {
        $input | Write-Output
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

function Error-Exit($message) {
    Write-ColorOutput $Red $message
    exit 1
}

function Section-Header($message) {
    Write-ColorOutput $Blue "`n=== $message ==="
}

# Get current directory and script path
$CURRENT_DIR = Get-Location
$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
$ROOT_DIR = Split-Path -Parent $SCRIPT_DIR

# Create build log directory
$LOGS_DIR = Join-Path -Path $ROOT_DIR -ChildPath "build_logs"
if (!(Test-Path $LOGS_DIR)) {
    New-Item -ItemType Directory -Path $LOGS_DIR | Out-Null
}

# Create state directory to track last build times
$STATE_DIR = Join-Path -Path $ROOT_DIR -ChildPath ".opencog-build-state"
if (!(Test-Path $STATE_DIR)) {
    New-Item -ItemType Directory -Path $STATE_DIR | Out-Null
}

# Function to check if a component has changed since last build
function Has-ComponentChanged {
    param (
        [string]$ComponentDir
    )
    
    $state_file = Join-Path -Path $STATE_DIR -ChildPath "$ComponentDir.timestamp"
    
    # If state file doesn't exist, component hasn't been built yet
    if (!(Test-Path $state_file)) {
        return $true
    }
    
    # Get last build time
    $last_build_time = Get-Content $state_file
    $last_build_date = [DateTime]::Parse($last_build_time)
    
    # Check if any file in the component directory has changed
    $changed_files = Get-ChildItem -Path $ComponentDir -Recurse -File | 
                     Where-Object { 
                         $_.LastWriteTime -gt $last_build_date -and 
                         $_.FullName -notlike "*\build\*" -and 
                         $_.FullName -notlike "*\.*" 
                     }
    
    if ($changed_files) {
        return $true
    }
    
    # Check if any dependency has been rebuilt since last build
    foreach ($dep in (Get-Dependencies $ComponentDir)) {
        $dep_state_file = Join-Path -Path $STATE_DIR -ChildPath "$dep.timestamp"
        if ((Test-Path $dep_state_file) -and 
            ((Get-Item $dep_state_file).LastWriteTime -gt $last_build_date)) {
            return $true
        }
    }
    
    return $false
}

# Function to get the dependencies of a component
function Get-Dependencies {
    param (
        [string]$ComponentDir
    )
    
    $dependencies = @()
    
    # Parse CMakeLists.txt to find dependencies
    $cmakefile = Join-Path -Path $ComponentDir -ChildPath "CMakeLists.txt"
    if (Test-Path $cmakefile) {
        # Extract project name
        $content = Get-Content $cmakefile
        $project_line = $content | Where-Object { $_ -match "project\s*\(" } | Select-Object -First 1
        if ($project_line) {
            $project_name = $project_line -replace ".*project\s*\(\s*([^ )]+).*", '$1'
            
            # Find components that this one depends on
            if ($project_name -eq "atomspace") {
                # AtomSpace depends only on cogutil
                $dependencies += "cogutil"
            }
            elseif ($project_name -eq "atomspace-rocks" -or $project_name -eq "atomspace-pgres") {
                # AtomSpace storage backends depend on atomspace and atomspace-storage
                $dependencies += "cogutil", "atomspace", "atomspace-storage"
            }
            elseif ($project_name -eq "cogserver") {
                # CogServer depends on atomspace
                $dependencies += "cogutil", "atomspace"
            }
            elseif ($project_name -eq "opencog") {
                # OpenCog depends on atomspace, cogserver, etc.
                $dependencies += "cogutil", "atomspace", "atomspace-storage", "atomspace-rocks", "cogserver"
            }
            elseif ($project_name -eq "learn") {
                # Learn depends on atomspace
                $dependencies += "cogutil", "atomspace"
            }
            elseif ($project_name -eq "ure") {
                # URE depends on atomspace
                $dependencies += "cogutil", "atomspace"
            }
            elseif ($project_name -eq "attention") {
                # Attention depends on atomspace, cogserver
                $dependencies += "cogutil", "atomspace", "cogserver"
            }
            elseif ($project_name -eq "sensory") {
                # Sensory depends on atomspace
                $dependencies += "cogutil", "atomspace"
            }
        }
    }
    
    return $dependencies
}

# Function to update the build timestamp of a component
function Update-BuildTimestamp {
    param (
        [string]$ComponentDir
    )
    
    $state_file = Join-Path -Path $STATE_DIR -ChildPath "$ComponentDir.timestamp"
    Set-Content -Path $state_file -Value (Get-Date -Format "yyyy-MM-dd HH:mm:ss")
}

# Function to build a component
function Build-Component {
    param (
        [string]$ComponentName,
        [string]$ComponentDir,
        [bool]$ForceBuild = $false
    )
    
    $log_file = Join-Path -Path $LOGS_DIR -ChildPath "${ComponentName}_build.log"
    
    # Check if component has changed or if force build is requested
    if (-not $ForceBuild -and -not (Has-ComponentChanged $ComponentDir)) {
        Write-ColorOutput $Green "Component $ComponentName is up to date, skipping."
        return
    }
    
    Write-ColorOutput $Yellow "Building $ComponentName..."
    
    # Check if component directory exists
    $component_path = Join-Path -Path $ROOT_DIR -ChildPath $ComponentDir
    if (!(Test-Path $component_path)) {
        Write-ColorOutput $Red "Directory $ComponentDir not found. Cloning from GitHub..."
        git clone "https://github.com/opencog/$ComponentName" $component_path
        if ($LASTEXITCODE -ne 0) {
            Error-Exit "Failed to clone $ComponentName repository."
        }
    }
    
    # Create build directory
    $build_dir = Join-Path -Path $component_path -ChildPath "build"
    if (!(Test-Path $build_dir)) {
        New-Item -ItemType Directory -Path $build_dir | Out-Null
    }
    
    # Configure, build and install
    Push-Location $build_dir
    
    Write-Output "Configuring $ComponentName..."
    cmake .. | Out-File -FilePath $log_file
    if ($LASTEXITCODE -ne 0) {
        Pop-Location
        Error-Exit "Failed to configure $ComponentName. See $log_file for details."
    }
    
    Write-Output "Building $ComponentName..."
    cmake --build . --config Release | Out-File -FilePath $log_file -Append
    if ($LASTEXITCODE -ne 0) {
        Pop-Location
        Error-Exit "Failed to build $ComponentName. See $log_file for details."
    }
    
    Write-Output "Installing $ComponentName..."
    cmake --install . | Out-File -FilePath $log_file -Append
    if ($LASTEXITCODE -ne 0) {
        Pop-Location
        Error-Exit "Failed to install $ComponentName. See $log_file for details."
    }
    
    Pop-Location
    
    # Update build timestamp
    Update-BuildTimestamp $ComponentDir
    
    Write-ColorOutput $Green "Successfully built and installed $ComponentName."
}

# Function to display build status for all components
function Display-BuildStatus {
    Write-ColorOutput $Blue "Build Status:"
    Write-ColorOutput $Blue "------------"
    
    $components = @("cogutil", "atomspace", "atomspace-storage", "atomspace-rocks", 
                   "atomspace-pgres", "cogserver", "learn", "opencog", "sensory", "evidence")
    
    foreach ($component in $components) {
        $state_file = Join-Path -Path $STATE_DIR -ChildPath "$component.timestamp"
        if (Test-Path $state_file) {
            $last_build_time = Get-Content $state_file
            if (Has-ComponentChanged $component) {
                Write-Host "$component: " -NoNewline
                Write-ColorOutput $Yellow "Needs rebuild (Last built: $last_build_time)"
            }
            else {
                Write-Host "$component: " -NoNewline
                Write-ColorOutput $Green "Up to date (Last built: $last_build_time)"
            }
        }
        else {
            Write-Host "$component: " -NoNewline
            Write-ColorOutput $Red "Never built"
        }
    }
}

# Main build process
function Incremental-Build {
    Section-Header "Analyzing components"
    
    # List of all components in the required build order
    $components = @(
        "cogutil",
        "atomspace",
        "unify",
        "ure",
        "atomspace-storage",
        "atomspace-rocks",
        "atomspace-pgres",
        "cogserver",
        "attention",
        "spacetime",
        "pln",
        "lg-atomese",
        "learn",
        "opencog",
        "sensory",
        "evidence"
    )
    
    $components_to_build = @()
    
    # Check which components need to be rebuilt
    foreach ($component in $components) {
        if (Has-ComponentChanged $component) {
            $components_to_build += $component
        }
    }
    
    if ($components_to_build.Count -eq 0) {
        Write-ColorOutput $Green "All components are up to date. Nothing to build."
        return
    }
    
    Write-ColorOutput $Yellow "The following components need to be rebuilt:"
    foreach ($component in $components_to_build) {
        Write-Output "  - $component"
    }
    
    # Build components in order
    Section-Header "Building components"
    foreach ($component in $components) {
        # Only build if component needs to be rebuilt
        if ($components_to_build -contains $component) {
            Build-Component $component $component $false
        }
    }
    
    Write-ColorOutput $Green "Incremental build completed successfully!"
}

# Parse command line arguments
param(
    [switch]$Status,
    [switch]$RebuildAll,
    [string]$Rebuild,
    [switch]$Help
)

if ($Help) {
    Write-Output "Usage: .\incremental-build.ps1 [-Status] [-RebuildAll] [-Rebuild component] [-Help]"
    Write-Output "Build OpenCog components incrementally."
    Write-Output ""
    Write-Output "Options:"
    Write-Output "  -Status         Display build status of all components"
    Write-Output "  -RebuildAll     Force rebuild of all components"
    Write-Output "  -Rebuild        Force rebuild of a specific component"
    Write-Output "  -Help           Display this help and exit"
    Write-Output ""
    exit 0
}

if ($Status) {
    Display-BuildStatus
    exit 0
}

if ($RebuildAll) {
    # Force rebuild all components
    Section-Header "Rebuilding all components"
    $components = @(
        "cogutil",
        "atomspace",
        "unify",
        "ure",
        "atomspace-storage",
        "atomspace-rocks",
        "atomspace-pgres",
        "cogserver",
        "attention",
        "spacetime",
        "pln",
        "lg-atomese",
        "learn",
        "opencog",
        "sensory",
        "evidence"
    )
    foreach ($component in $components) {
        Build-Component $component $component $true
    }
    Write-ColorOutput $Green "Full rebuild completed successfully!"
    exit 0
}

if ($Rebuild) {
    Build-Component $Rebuild $Rebuild $true
    exit 0
}

Incremental-Build 