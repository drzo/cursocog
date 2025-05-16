# Directories to process
$directories = @(
    "atomspace",
    "atomspace-pgres",
    "atomspace-rocks",
    "atomspace-storage",
    "cogserver",
    "cogutil",
    "docker",
    "evidence",
    "learn",
    "opencog",
    "profile",
    "sensory"
)

# 1. Find and remove any .git directories in subdirectories
Write-Host "Removing any remaining .git directories..." -ForegroundColor Green
Get-ChildItem -Path . -Recurse -Directory -Filter .git | 
    Where-Object { $_.FullName -ne (Join-Path (Get-Location) ".git") } |
    ForEach-Object {
        Write-Host "Removing $($_.FullName)" -ForegroundColor Yellow
        Remove-Item -Path $_.FullName -Recurse -Force
    }

# 2. Reset the git state for each directory to ensure they're regular directories
Write-Host "`nResetting git state for all directories..." -ForegroundColor Green
foreach ($dir in $directories) {
    if (Test-Path $dir) {
        Write-Host "Removing $dir from git index..." -ForegroundColor Yellow
        git rm --cached -r $dir 2>$null
        
        Write-Host "Adding $dir back as regular directory..." -ForegroundColor Yellow
        git add $dir
    }
}

# 3. Commit the changes
Write-Host "`nCommitting changes..." -ForegroundColor Green
git commit -m "Convert all repositories to regular directories, removing all external source control references"

Write-Host "`nComplete! All directories are now disconnected from external source control." -ForegroundColor Green
Write-Host "You can now safely back up this directory without affecting external source control." -ForegroundColor Green 