# CogInit - Windows Dependencies Installer
# Installs all prerequisites for OpenCog development on Windows

param (
    [switch] = False,
    [switch] = False,
    [switch] = False
)

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

Write-Host "CogInit - OpenCog Prerequisites Installer (Windows)" -ForegroundColor Cyan
Write-Host "===================================================" -ForegroundColor Cyan
Write-Host ""

# Get the current location
 = Split-Path -Parent System.Management.Automation.InvocationInfo.MyCommand.Path
 = Split-Path -Parent 
 = Join-Path -Path  -ChildPath "deps"

# Load dependency information
coginit\dependencies.json = Join-Path -Path  -ChildPath "dependencies.json"
if (!(Test-Path coginit\dependencies.json)) {
    Write-ColorOutput Red "Dependencies configuration file not found: coginit\dependencies.json"
    exit 1
}

System.Collections.Hashtable System.Collections.Hashtable System.Collections.Hashtable System.Collections.Hashtable System.Collections.Hashtable System.Collections.Hashtable System.Collections.Hashtable = Get-Content -Raw -Path coginit\dependencies.json | ConvertFrom-Json

# Create installation directory
 = Join-Path -Path C:\Users\Ewok\AppData\Local\Temp -ChildPath "coginit_install"
if (!(Test-Path )) {
    New-Item -ItemType Directory -Path  | Out-Null
}

# Install each dependency
foreach (System.Collections.Hashtable in System.Collections.Hashtable System.Collections.Hashtable System.Collections.Hashtable System.Collections.Hashtable System.Collections.Hashtable System.Collections.Hashtable System.Collections.Hashtable) {
    if (!( -and !(System.Collections.Hashtable.Essential)) -or ) {
        Write-ColorOutput Yellow "Processing doxygen v1.9.6..."
        
        # Check if the user wants to install this dependency
         = True
        if (! -and ! -and !) {
             = Read-Host "Install doxygen? (Y/n)"
            if ( -eq "n" -or  -eq "N") {
                 = False
            }
        }
        
        if () {
            # Extract the archive
             = Join-Path -Path  -ChildPath System.Collections.Hashtable.FileName
             = Join-Path -Path  -ChildPath System.Collections.Hashtable.Name
            
            if (!(Test-Path )) {
                Write-ColorOutput Red "Archive not found: "
                continue
            }
            
            # Create extraction directory
            if (!(Test-Path )) {
                New-Item -ItemType Directory -Path  | Out-Null
            }
            
            # Extract based on file extension
            if (System.Collections.Hashtable.FileName -match "\.zip$") {
                Write-Host "  Extracting ZIP archive..."
                Expand-Archive -Path  -DestinationPath  -Force
            }
            elseif (System.Collections.Hashtable.FileName -match "\.tar\.gz$") {
                Write-Host "  Extracting TAR.GZ archive..."
                # Use 7-Zip if available, otherwise use built-in tar
                if (Get-Command "7z" -ErrorAction SilentlyContinue) {
                    & 7z x  -o"" -y
                     = Get-ChildItem -Path  -Filter "*.tar" | Select-Object -First 1
                    if () {
                        & 7z x .FullName -o"" -y
                        Remove-Item .FullName -Force
                    }
                }
                else {
                    tar -xzf  -C 
                }
            }
            
            # Perform dependency-specific installation steps
            switch (System.Collections.Hashtable.Name) {
                "boost" {
                    Write-Host "  Boost requires separate build steps. See ./docs/boost_windows.md for instructions."
                }
                "cmake" {
                    Write-Host "  Adding CMake to PATH environment variable..."
                     = Get-ChildItem -Path  -Directory | Select-Object -First 1
                     = Join-Path -Path .FullName -ChildPath "bin"
                    C:\Program Files\PowerShell\7;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Python313\Scripts\;C:\Python313\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Program Files\dotnet\;C:\Program Files\nodejs\;C:\ProgramData\chocolatey\bin;C:\Program Files\Hashicorp Boundary\;C:\Users\Ewok\AppData\Local\Microsoft\WindowsApps;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Program Files (x86)\AOMEI\AOMEI Backupper\6.5.1;C:\Program Files\Git\cmd;C:\Program Files\PowerShell\7\;C:\Program Files\Microsoft VS Code\bin;C:\Program Files\cursor\resources\app\bin;C:\Program Files\CMake\bin;C:\Users\Ewok\scoop\shims;C:\Users\Ewok\.cargo\bin;C:\Users\Ewok\.local\bin;C:\Users\Ewok\.console-ninja\.bin;C:\Users\Ewok\AppData\Local\pnpm;C:\Users\Ewok\AppData\Local\GitHubDesktop\bin;C:\Users\Ewok\AppData\Local\Microsoft\WinGet\Links;C:\Users\Ewok\AppData\Local\Programs\Windsurf\bin;C:\Users\Ewok\AppData\Local\Programs\VSCodium\bin;C:\Users\Ewok\AppData\Local\Programs\VSCodium Insiders\bin =  + ";" + C:\Program Files\PowerShell\7;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Python313\Scripts\;C:\Python313\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Program Files\dotnet\;C:\Program Files\nodejs\;C:\ProgramData\chocolatey\bin;C:\Program Files\Hashicorp Boundary\;C:\Users\Ewok\AppData\Local\Microsoft\WindowsApps;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Program Files (x86)\AOMEI\AOMEI Backupper\6.5.1;C:\Program Files\Git\cmd;C:\Program Files\PowerShell\7\;C:\Program Files\Microsoft VS Code\bin;C:\Program Files\cursor\resources\app\bin;C:\Program Files\CMake\bin;C:\Users\Ewok\scoop\shims;C:\Users\Ewok\.cargo\bin;C:\Users\Ewok\.local\bin;C:\Users\Ewok\.console-ninja\.bin;C:\Users\Ewok\AppData\Local\pnpm;C:\Users\Ewok\AppData\Local\GitHubDesktop\bin;C:\Users\Ewok\AppData\Local\Microsoft\WinGet\Links;C:\Users\Ewok\AppData\Local\Programs\Windsurf\bin;C:\Users\Ewok\AppData\Local\Programs\VSCodium\bin;C:\Users\Ewok\AppData\Local\Programs\VSCodium Insiders\bin
                    [Environment]::SetEnvironmentVariable("PATH", C:\Program Files\PowerShell\7;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Python313\Scripts\;C:\Python313\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Program Files\dotnet\;C:\Program Files\nodejs\;C:\ProgramData\chocolatey\bin;C:\Program Files\Hashicorp Boundary\;C:\Users\Ewok\AppData\Local\Microsoft\WindowsApps;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Program Files (x86)\AOMEI\AOMEI Backupper\6.5.1;C:\Program Files\Git\cmd;C:\Program Files\PowerShell\7\;C:\Program Files\Microsoft VS Code\bin;C:\Program Files\cursor\resources\app\bin;C:\Program Files\CMake\bin;C:\Users\Ewok\scoop\shims;C:\Users\Ewok\.cargo\bin;C:\Users\Ewok\.local\bin;C:\Users\Ewok\.console-ninja\.bin;C:\Users\Ewok\AppData\Local\pnpm;C:\Users\Ewok\AppData\Local\GitHubDesktop\bin;C:\Users\Ewok\AppData\Local\Microsoft\WinGet\Links;C:\Users\Ewok\AppData\Local\Programs\Windsurf\bin;C:\Users\Ewok\AppData\Local\Programs\VSCodium\bin;C:\Users\Ewok\AppData\Local\Programs\VSCodium Insiders\bin, "User")
                    & cmake --version
                }
                "cxxtest" {
                    Write-Host "  Setting CXXTEST_HOME environment variable..."
                     = Get-ChildItem -Path  -Directory | Select-Object -First 1
                    [Environment]::SetEnvironmentVariable("CXXTEST_HOME", .FullName, "User")
                }
                "doxygen" {
                    Write-Host "  Adding Doxygen to PATH environment variable..."
                     = Get-ChildItem -Path  -Directory | Select-Object -First 1
                    [Environment]::SetEnvironmentVariable("PATH", .FullName + ";" + C:\Program Files\PowerShell\7;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Python313\Scripts\;C:\Python313\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Program Files\dotnet\;C:\Program Files\nodejs\;C:\ProgramData\chocolatey\bin;C:\Program Files\Hashicorp Boundary\;C:\Users\Ewok\AppData\Local\Microsoft\WindowsApps;c:\Users\sandbox719\AppData\Local\Programs\cursor\resources\app\bin;C:\Program Files (x86)\AOMEI\AOMEI Backupper\6.5.1;C:\Program Files\Git\cmd;C:\Program Files\PowerShell\7\;C:\Program Files\Microsoft VS Code\bin;C:\Program Files\cursor\resources\app\bin;C:\Program Files\CMake\bin;C:\Users\Ewok\scoop\shims;C:\Users\Ewok\.cargo\bin;C:\Users\Ewok\.local\bin;C:\Users\Ewok\.console-ninja\.bin;C:\Users\Ewok\AppData\Local\pnpm;C:\Users\Ewok\AppData\Local\GitHubDesktop\bin;C:\Users\Ewok\AppData\Local\Microsoft\WinGet\Links;C:\Users\Ewok\AppData\Local\Programs\Windsurf\bin;C:\Users\Ewok\AppData\Local\Programs\VSCodium\bin;C:\Users\Ewok\AppData\Local\Programs\VSCodium Insiders\bin, "User")
                    & doxygen --version
                }
                default {
                    Write-Host "  No specific installation steps needed for doxygen."
                }
            }
            
            Write-ColorOutput Green "Installed doxygen v1.9.6"
        }
        else {
            Write-Host "Skipping doxygen..."
        }
    }
}

Write-Host ""
Write-ColorOutput Green "OpenCog prerequisites installation completed!"
Write-Host "Some dependencies may require additional configuration. See docs/ for details."
