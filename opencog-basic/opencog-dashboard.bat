@echo off
rem OpenCog Dashboard Launcher
rem This script launches the Clay UI dashboard for OpenCog Basic

echo Launching OpenCog Dashboard...

rem Check if dashboard exists
if exist ".\clay-ui\build\dashboard.exe" (
    rem Launch dashboard
    .\clay-ui\build\dashboard.exe %*
) else (
    echo Dashboard executable not found.
    echo Building dashboard...
    
    rem Try to build it
    if exist ".\integrate-clay-ui.ps1" (
        echo Running Clay UI integration...
        powershell -ExecutionPolicy Bypass -File .\integrate-clay-ui.ps1
        
        rem Check if build succeeded
        if exist ".\clay-ui\build\dashboard.exe" (
            echo Dashboard built successfully.
            rem Launch dashboard
            .\clay-ui\build\dashboard.exe %*
        ) else (
            echo Failed to build dashboard.
            echo Please run '.\integrate-clay-ui.ps1' manually.
            exit /b 1
        )
    ) else (
        echo Integration script not found.
        echo Please ensure you are in the opencog-basic directory.
        exit /b 1
    )
) 