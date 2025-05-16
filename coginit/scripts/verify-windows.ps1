# CogInit - Windows Dependencies Verification
# Verifies all prerequisites for OpenCog development on Windows

# Define console colors
Green = [System.ConsoleColor]::Green
Red = [System.ConsoleColor]::Red
Yellow = [System.ConsoleColor]::Yellow

function Write-ColorOutput() {
     = System.Management.Automation.Internal.Host.InternalHost.UI.RawUI.ForegroundColor
    System.Management.Automation.Internal.Host.InternalHost.UI.RawUI.ForegroundColor = 
    if () {
        Write-Output 
    }
    else {
         | Write-Output
    }
    System.Management.Automation.Internal.Host.InternalHost.UI.RawUI.ForegroundColor = 
}

Write-Host "CogInit - OpenCog Prerequisites Verification (Windows)" -ForegroundColor Cyan
Write-Host "=====================================================" -ForegroundColor Cyan
Write-Host ""

# Define tests for dependencies
 = @(
    @{
        Name = "CMake";
        Command = "cmake --version";
        Pattern = "cmake version (\d+\.\d+\.\d+)";
        MinimumVersion = "3.12.0";
    },
    @{
        Name = "Boost";
        Command = "if (Test-Path ) { echo "Boost found at " } else { echo "Boost not found" }";
        Pattern = "Boost found at (.+)";
        Required = True;
    },
    @{
        Name = "CxxTest";
        Command = "if (Test-Path ) { echo "CxxTest found at " } else { echo "CxxTest not found" }";
        Pattern = "CxxTest found at (.+)";
        Required = True;
    },
    @{
        Name = "Doxygen";
        Command = "try { doxygen --version } catch { echo "Doxygen not found" }";
        Pattern = "(\d+\.\d+\.\d+)";
        MinimumVersion = "1.8.0";
        Required = False;
    }
)

# Display table header
 = "{0,-15} {1,-15} {2,-30} {3,-10}"
Write-Host ( -f "Dependency", "Required", "Status", "Version")
Write-Host ( -f "-----------", "--------", "------", "-------")

# Run each test
foreach ( in ) {
     = if (.Required -eq True) { "Yes" } else { "No" }
    
    try {
        # Execute the command
         = Invoke-Expression .Command | Out-String
        
        # Check for pattern match
        if (.Pattern -and  -match .Pattern) {
             = [1]
            
            # Check version if needed
            if (.MinimumVersion) {
                if ([System.Version] -ge [System.Version].MinimumVersion) {
                    Write-Host ( -f .Name, , "Installed", ) -ForegroundColor Green
                }
                else {
                    Write-Host ( -f .Name, , "Version too old", ) -ForegroundColor Red
                }
            }
            else {
                Write-Host ( -f .Name, , "Installed", ) -ForegroundColor Green
            }
        }
        else {
            if (.Required -eq True) {
                Write-Host ( -f .Name, , "Not found", "N/A") -ForegroundColor Red
            }
            else {
                Write-Host ( -f .Name, , "Not found", "N/A") -ForegroundColor Yellow
            }
        }
    }
    catch {
        if (.Required -eq True) {
            Write-Host ( -f .Name, , "Error checking", "N/A") -ForegroundColor Red
        }
        else {
            Write-Host ( -f .Name, , "Error checking", "N/A") -ForegroundColor Yellow
        }
    }
}

Write-Host ""
Write-Host "Verification complete. Make sure all required dependencies are installed."
Write-Host "For any missing components, run ./scripts/install-windows.ps1"
