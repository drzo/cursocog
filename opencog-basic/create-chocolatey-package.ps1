#
# OpenCog Basic - Chocolatey Package Creation Script
# This script creates a Chocolatey package for OpenCog Basic
#

Write-Host "OpenCog Basic - Chocolatey Package Creation" -ForegroundColor Cyan
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host ""

# Check if Chocolatey is installed
function Check-Chocolatey {
    if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
        Write-Host "Chocolatey is not installed. Please install it first:" -ForegroundColor Red
        Write-Host "Set-ExecutionPolicy Bypass -Scope Process -Force" -ForegroundColor Yellow
        Write-Host "[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072" -ForegroundColor Yellow
        Write-Host "iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))" -ForegroundColor Yellow
        return $false
    }
    
    Write-Host "Chocolatey is installed." -ForegroundColor Green
    return $true
}

# Check if Windows installer exists or create it
function Prepare-Installer {
    $installerPath = "$PSScriptRoot/build/OpenCog-Basic-0.1.0-Setup.exe"
    
    if (Test-Path $installerPath) {
        Write-Host "Windows installer already exists at $installerPath" -ForegroundColor Green
    } else {
        Write-Host "Windows installer not found. Creating it..." -ForegroundColor Yellow
        
        # Run the Windows installer creation script
        if (Test-Path "$PSScriptRoot/create-windows-installer.ps1") {
            & "$PSScriptRoot/create-windows-installer.ps1"
            
            if (-not (Test-Path $installerPath)) {
                Write-Host "Failed to create Windows installer." -ForegroundColor Red
                return $false
            }
        } else {
            Write-Host "Windows installer creation script not found at $PSScriptRoot/create-windows-installer.ps1" -ForegroundColor Red
            return $false
        }
    }
    
    return $installerPath
}

# Create Chocolatey package
function Create-ChocoPackage {
    param (
        [string]$installerPath
    )
    
    # Calculate checksum for the installer
    Write-Host "Calculating checksum for installer..." -ForegroundColor Yellow
    $checksum = Get-FileHash -Algorithm SHA256 -Path $installerPath | Select-Object -ExpandProperty Hash
    Write-Host "Checksum: $checksum" -ForegroundColor Green
    
    # Create directory structure for the package
    $chocoDir = "$PSScriptRoot/build/chocolatey/opencog-basic"
    $toolsDir = "$chocoDir/tools"
    
    Write-Host "Creating package directory structure..." -ForegroundColor Yellow
    if (Test-Path $chocoDir) {
        Remove-Item -Path $chocoDir -Recurse -Force
    }
    New-Item -ItemType Directory -Path $toolsDir -Force | Out-Null
    
    # Create nuspec file
    Write-Host "Creating nuspec file..." -ForegroundColor Yellow
    $nuspecContent = @"
<?xml version="1.0" encoding="utf-8"?>
<package xmlns="http://schemas.microsoft.com/packaging/2015/06/nuspec.xsd">
  <metadata>
    <id>opencog-basic</id>
    <version>0.1.0</version>
    <title>OpenCog Basic</title>
    <authors>OpenCog Foundation</authors>
    <projectUrl>https://github.com/opencog/opencog-basic</projectUrl>
    <licenseUrl>https://github.com/opencog/opencog/blob/master/LICENSE</licenseUrl>
    <requireLicenseAcceptance>true</requireLicenseAcceptance>
    <tags>opencog atomspace ai cognitive-architecture</tags>
    <summary>Core components of the OpenCog framework</summary>
    <description>
OpenCog Basic includes the essential components for working with the
AtomSpace knowledge representation framework and related tools.

Components included:
- AtomSpace - Knowledge representation framework
- AtomSpace Storage (RocksDB backend) - Persistence capabilities
- CogServer - Network server for AtomSpace access
- Clay UI - Visualization and interaction tools
    </description>
  </metadata>
</package>
"@
    $nuspecContent | Out-File -FilePath "$chocoDir/opencog-basic.nuspec" -Encoding UTF8
    
    # Create chocolateyinstall.ps1
    Write-Host "Creating chocolateyinstall.ps1..." -ForegroundColor Yellow
    $installScript = @"
`$ErrorActionPreference = 'Stop'
`$toolsDir   = "`$(Split-Path -parent `$MyInvocation.MyCommand.Definition)"
`$url        = 'https://github.com/opencog/opencog-basic/releases/download/v0.1.0/OpenCog-Basic-0.1.0-Setup.exe'

`$packageArgs = @{
  packageName   = `$env:ChocolateyPackageName
  unzipLocation = `$toolsDir
  fileType      = 'EXE'
  url           = `$url
  softwareName  = 'OpenCog Basic*'
  checksum      = '$checksum'
  checksumType  = 'sha256'
  silentArgs    = '/S'
  validExitCodes= @(0)
}

Install-ChocolateyPackage @packageArgs
"@
    $installScript | Out-File -FilePath "$toolsDir/chocolateyinstall.ps1" -Encoding UTF8
    
    # Create chocolateyuninstall.ps1
    Write-Host "Creating chocolateyuninstall.ps1..." -ForegroundColor Yellow
    $uninstallScript = @"
`$ErrorActionPreference = 'Stop'
`$packageArgs = @{
  packageName   = `$env:ChocolateyPackageName
  softwareName  = 'OpenCog Basic*'
  fileType      = 'EXE'
  silentArgs    = '/S'
  validExitCodes= @(0)
}

`$uninstalled = `$false
[array]`$key = Get-UninstallRegistryKey -SoftwareName `$packageArgs['softwareName']

if (`$key.Count -eq 1) {
  `$key | ForEach-Object { 
    `$packageArgs['file'] = "`$(`$_.UninstallString)"
    Uninstall-ChocolateyPackage @packageArgs
  }
} elseif (`$key.Count -eq 0) {
  Write-Warning "`$(`$packageArgs['packageName']) has already been uninstalled by other means."
} elseif (`$key.Count -gt 1) {
  Write-Warning "`$(`$key.Count) matches found!"
  Write-Warning "To prevent accidental data loss, no programs will be uninstalled."
}
"@
    $uninstallScript | Out-File -FilePath "$toolsDir/chocolateyuninstall.ps1" -Encoding UTF8
    
    # Build package
    Write-Host "Building Chocolatey package..." -ForegroundColor Yellow
    Push-Location $chocoDir
    choco pack
    $result = $LASTEXITCODE
    Pop-Location
    
    if ($result -eq 0) {
        # Move the package to the build directory
        $nupkgFile = "$chocoDir/opencog-basic.0.1.0.nupkg"
        if (Test-Path $nupkgFile) {
            Write-Host "Chocolatey package created: $nupkgFile" -ForegroundColor Green
            return $nupkgFile
        } else {
            Write-Host "Package was built but could not be found." -ForegroundColor Red
            return $false
        }
    } else {
        Write-Host "Failed to build Chocolatey package." -ForegroundColor Red
        return $false
    }
}

# Main execution
if (-not (Check-Chocolatey)) {
    exit 1
}

$installerPath = Prepare-Installer
if (-not $installerPath) {
    exit 1
}

$packagePath = Create-ChocoPackage -installerPath $installerPath
if (-not $packagePath) {
    exit 1
}

Write-Host ""
Write-Host "Chocolatey package creation completed successfully." -ForegroundColor Green
Write-Host "Package location: $packagePath" -ForegroundColor Green
Write-Host ""
Write-Host "To test the package locally: " -ForegroundColor Yellow
Write-Host "choco install opencog-basic -s $PSScriptRoot\build\chocolatey" -ForegroundColor Yellow
Write-Host ""
Write-Host "To publish to Chocolatey Community Repository: " -ForegroundColor Yellow
Write-Host "choco apikey --key YOUR_API_KEY --source https://push.chocolatey.org/" -ForegroundColor Yellow
Write-Host "choco push $packagePath --source https://push.chocolatey.org/" -ForegroundColor Yellow 