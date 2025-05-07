Get-ChildItem -Directory | ForEach-Object { 
    $gitDir = Join-Path $_.FullName ".git"
    if (Test-Path $gitDir -PathType Container) {
        Write-Host "Removing .git from $($_.Name)"
        Remove-Item -Recurse -Force $gitDir
    }
} 