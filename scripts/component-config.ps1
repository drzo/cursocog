param(
    [Parameter(Position=0)]
    [string]$Command,
    
    [Parameter(Position=1)]
    [string]$Component,
    
    [Parameter(Position=2)]
    [string]$Option,
    
    [Parameter(Position=3)]
    [string]$Value,
    
    [switch]$Help
)

# OpenCog Component Configuration System
# This script manages component-specific configuration options

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

# Configuration directory
$CONFIG_DIR = ".opencog-config"
if (!(Test-Path $CONFIG_DIR)) {
    New-Item -ItemType Directory -Path $CONFIG_DIR | Out-Null
}

# Function to get configuration for a component
function Get-ComponentConfig($component) {
    $configFile = Join-Path -Path $CONFIG_DIR -ChildPath "${component}.conf"
    
    if (Test-Path $configFile) {
        Get-Content $configFile
    } else {
        "# Default configuration for $component"
        "# Uncomment and modify options as needed"
        
        # Default options for each component
        switch ($component) {
            "cogutil" {
                "#BUILD_SHARED_LIBS=ON"
                "#BUILD_TESTS=ON"
            }
            "atomspace" {
                "#BUILD_SHARED_LIBS=ON"
                "#BUILD_TESTS=ON"
                "#CXXTEST_BIN_DIR=C:\Program Files\CxxTest\bin"
            }
            "atomspace-rocks" {
                "#BUILD_SHARED_LIBS=ON"
                "#ROCKSDB_INCLUDE_DIR=C:\Program Files\RocksDB\include"
                "#ROCKSDB_LIBRARY_DIR=C:\Program Files\RocksDB\lib"
            }
            "atomspace-pgres" {
                "#BUILD_SHARED_LIBS=ON"
                "#PGSQL_INCLUDE_DIR=C:\Program Files\PostgreSQL\13\include"
                "#PGSQL_LIBRARY_DIR=C:\Program Files\PostgreSQL\13\lib"
            }
            "cogserver" {
                "#BUILD_SHARED_LIBS=ON"
                "#ENABLE_NETWORK_SERVER=ON"
            }
            "learn" {
                "#BUILD_SHARED_LIBS=ON"
                "#LINK_GRAMMAR_INCLUDE_DIR=C:\Program Files\LinkGrammar\include"
                "#LINK_GRAMMAR_LIBRARY_DIR=C:\Program Files\LinkGrammar\lib"
            }
            "opencog" {
                "#BUILD_SHARED_LIBS=ON"
                "#ENABLE_LEARNING=ON"
                "#ENABLE_PLN=ON"
                "#ENABLE_ATTENTION=ON"
            }
            "sensory" {
                "#BUILD_SHARED_LIBS=ON"
                "#ENABLE_VISION=ON"
                "#ENABLE_AUDIO=ON"
            }
            default {
                "#BUILD_SHARED_LIBS=ON"
                "#BUILD_TESTS=ON"
            }
        }
    }
}

# Function to set configuration for a component
function Set-ComponentConfig($component, $option, $value) {
    $configFile = Join-Path -Path $CONFIG_DIR -ChildPath "${component}.conf"
    
    # Create config file if it doesn't exist
    if (!(Test-Path $configFile)) {
        Get-ComponentConfig $component | Out-File -FilePath $configFile
    }
    
    $content = Get-Content $configFile
    $updated = $false
    
    # Check if option already exists
    for ($i = 0; $i -lt $content.Count; $i++) {
        if ($content[$i] -match "^#?$option=.*") {
            $content[$i] = "$option=$value"
            $updated = $true
            break
        }
    }
    
    # Add new option if not found
    if (!$updated) {
        $content += "$option=$value"
    }
    
    # Write updated content
    $content | Out-File -FilePath $configFile
    
    Write-ColorOutput $Green "Set ${option}=${value} for ${component}"
}

# Function to generate CMake arguments from config
function Generate-CMakeArgs($component) {
    $configFile = Join-Path -Path $CONFIG_DIR -ChildPath "${component}.conf"
    $cmakeArgs = ""
    
    if (Test-Path $configFile) {
        foreach ($line in Get-Content $configFile) {
            # Skip comments and empty lines
            if ($line -notmatch "^#" -and $line -match "=") {
                $keyValue = $line -split "=", 2
                $key = $keyValue[0].Trim()
                $value = $keyValue[1].Trim()
                
                if ($key -and $value) {
                    $cmakeArgs += " -D$key=$value"
                }
            }
        }
    }
    
    return $cmakeArgs
}

# Function to list available components
function List-Components {
    Write-ColorOutput $Blue "Available components:"
    "cogutil"
    "atomspace"
    "unify"
    "ure"
    "atomspace-storage"
    "atomspace-rocks"
    "atomspace-pgres"
    "cogserver"
    "attention"
    "spacetime"
    "pln"
    "lg-atomese"
    "learn"
    "opencog"
    "sensory"
    "evidence"
}

# Function to display configuration for a component
function Show-ComponentConfig($component) {
    $configFile = Join-Path -Path $CONFIG_DIR -ChildPath "${component}.conf"
    
    Write-ColorOutput $Blue "Configuration for ${component}:"
    
    if (Test-Path $configFile) {
        Get-Content $configFile
    } else {
        "No custom configuration set. Using defaults."
        Get-ComponentConfig $component
    }
}

if ($Help -or !$Command) {
    Write-Output "Usage: .\component-config.ps1 <command> [options]"
    Write-Output "Manage component-specific configuration options."
    Write-Output ""
    Write-Output "Commands:"
    Write-Output "  get <component>                  Get configuration template for a component"
    Write-Output "  set <component> <option> <value> Set configuration option for a component"
    Write-Output "  show <component>                 Show current configuration for a component"
    Write-Output "  args <component>                 Generate CMake arguments for a component"
    Write-Output "  list                             List available components"
    Write-Output "  -Help                            Display this help and exit"
    Write-Output ""
    exit 0
}

switch ($Command) {
    "get" {
        if (!$Component) {
            Error-Exit "Please specify a component name."
        }
        Get-ComponentConfig $Component
    }
    "set" {
        if (!$Component -or !$Option -or !$Value) {
            Error-Exit "Usage: .\component-config.ps1 set <component> <option> <value>"
        }
        Set-ComponentConfig $Component $Option $Value
    }
    "show" {
        if (!$Component) {
            Error-Exit "Please specify a component name."
        }
        Show-ComponentConfig $Component
    }
    "args" {
        if (!$Component) {
            Error-Exit "Please specify a component name."
        }
        Generate-CMakeArgs $Component
    }
    "list" {
        List-Components
    }
    default {
        Error-Exit "Unknown command. Use -Help for usage information."
    }
} 