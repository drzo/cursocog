# Creating Chocolatey Packages for OpenCog

This guide outlines the steps to create a Chocolatey package for OpenCog components. Chocolatey is a package manager for Windows that makes software installation and management easier.

## Prerequisites

- Windows system with administrative privileges
- Chocolatey installed (see https://chocolatey.org/install)
- NSIS installed (for the Windows installer creation)
- OpenCog source code and built components

## Step 1: Install Chocolatey (if not already installed)

```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

## Step 2: Create Windows Installer

Before creating a Chocolatey package, you need a Windows installer. Use the existing `create-windows-installer.ps1` script:

```powershell
.\opencog-basic\create-windows-installer.ps1
```

This will create a Windows installer at `opencog-basic\build\OpenCog-Basic-0.1.0-Setup.exe`.

## Step 3: Create Package Scaffold

```powershell
# Create directory for package development
mkdir choco-opencog
cd choco-opencog

# Create package scaffold
choco new opencog-basic
```

## Step 4: Edit Package Metadata

Edit the file `opencog-basic\opencog-basic.nuspec`:

```xml
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
```

## Step 5: Create Installation Script

Create the file `opencog-basic\tools\chocolateyinstall.ps1`:

```powershell
$ErrorActionPreference = 'Stop'
$toolsDir   = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url        = 'https://github.com/opencog/opencog-basic/releases/download/v0.1.0/OpenCog-Basic-0.1.0-Setup.exe'

# Calculate checksum if you have the installer file
# $checksum = Get-FileHash -Algorithm SHA256 -Path "path\to\OpenCog-Basic-0.1.0-Setup.exe" | Select-Object -ExpandProperty Hash

$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  unzipLocation = $toolsDir
  fileType      = 'EXE'
  url           = $url
  softwareName  = 'OpenCog Basic*'
  
  # Uncomment and add checksum when you have a released installer
  # checksum      = 'THE-CHECKSUM-VALUE'
  # checksumType  = 'sha256'
  
  # NSIS silent install parameter
  silentArgs    = '/S'
  validExitCodes= @(0)
}

Install-ChocolateyPackage @packageArgs
```

## Step 6: Create Uninstallation Script

Create the file `opencog-basic\tools\chocolateyuninstall.ps1`:

```powershell
$ErrorActionPreference = 'Stop'
$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  softwareName  = 'OpenCog Basic*'
  fileType      = 'EXE'
  silentArgs    = '/S' # NSIS silent uninstall parameter
  validExitCodes= @(0)
}

$uninstalled = $false
[array]$key = Get-UninstallRegistryKey -SoftwareName $packageArgs['softwareName']

if ($key.Count -eq 1) {
  $key | ForEach-Object { 
    $packageArgs['file'] = "$($_.UninstallString)"
    Uninstall-ChocolateyPackage @packageArgs
  }
} elseif ($key.Count -eq 0) {
  Write-Warning "$($packageArgs['packageName']) has already been uninstalled by other means."
} elseif ($key.Count -gt 1) {
  Write-Warning "$($key.Count) matches found!"
  Write-Warning "To prevent accidental data loss, no programs will be uninstalled."
}
```

## Step 7: Build and Test the Package

```powershell
# Navigate to the package directory
cd opencog-basic

# Build the package
choco pack

# Test install locally (requires admin privileges)
choco install opencog-basic -s .
```

## Step 8: Automated Package Creation

For automated package creation during your build process, create a script like this:

```powershell
# create-chocolatey-package.ps1

# 1. First build the NSIS installer
Write-Host "Building Windows installer..." -ForegroundColor Yellow
./opencog-basic/create-windows-installer.ps1

# 2. Calculate checksum
$installerPath = "opencog-basic/build/OpenCog-Basic-0.1.0-Setup.exe"
$checksum = Get-FileHash -Algorithm SHA256 -Path $installerPath | Select-Object -ExpandProperty Hash

# 3. Create Chocolatey package directory
$chocoDir = "build/chocolatey/opencog-basic"
$toolsDir = "$chocoDir/tools"
New-Item -ItemType Directory -Path $toolsDir -Force | Out-Null

# 4. Create nuspec file
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

# 5. Create chocolateyinstall.ps1
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

# 6. Create chocolateyuninstall.ps1
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

# 7. Build package
Set-Location -Path $chocoDir
choco pack

Write-Host "Chocolatey package created: opencog-basic.0.1.0.nupkg" -ForegroundColor Green
```

## Publishing to Chocolatey Community Repository

If you want to publish the package to the Chocolatey Community Repository:

1. Create an account at https://community.chocolatey.org/
2. Get your API key from your account page
3. Set your API key:
   ```powershell
   choco apikey --key your-api-key --source https://push.chocolatey.org/
   ```
4. Push your package:
   ```powershell
   choco push opencog-basic.0.1.0.nupkg --source https://push.chocolatey.org/
   ```

## Additional Resources

- [Chocolatey Package Documentation](https://docs.chocolatey.org/en-us/features/create-packages/)
- [Creating Chocolatey Packages Step-By-Step](https://blog.chocolatey.org/2024/03/creating-chocolatey-packages-step-by-step/) 