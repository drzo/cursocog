# Directories that appear as submodules
$submoduleDirs = @(
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

foreach ($dir in $submoduleDirs) {
    Write-Host "Processing $dir..."
    
    # Remove from Git's index
    git rm --cached $dir 2>$null

    # Add back as regular directory
    if (Test-Path $dir -PathType Container) {
        Write-Host "Adding $dir back to Git..."
        git add $dir 2>$null
    }
}

# Commit the changes
Write-Host "Committing changes..."
git commit -m "Convert submodules to regular directories"

# Push the changes
Write-Host "Pushing changes to GitHub..."
git push origin master 