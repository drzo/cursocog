$sourceFiles = Get-ChildItem -Path . -Filter "*.cc" | Where-Object { $_.Name -ne "platform.cc" -and $_.Name -ne "tree.cc" }

foreach ($file in $sourceFiles) {
    $content = Get-Content $file.FullName
    $firstLine = '#include "platform.h"'
    
    if ($content[0] -ne $firstLine) {
        $newContent = @($firstLine) + $content
        Set-Content -Path $file.FullName -Value $newContent
        Write-Host "Added platform.h include to $($file.Name)"
    } else {
        Write-Host "$($file.Name) already has platform.h include"
    }
}

Write-Host "Done fixing includes!" 