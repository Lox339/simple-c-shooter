@echo off
REM Quick build script for Windows development
REM This script performs a minimal build to check for compilation errors

setlocal enabledelayedexpansion

echo [INFO] Quick Build Script for Simple C Shooter
echo.

REM Check for CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake is required but not installed
    echo Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

REM Check for Visual Studio Build Tools
where msbuild >nul 2>&1
if errorlevel 1 (
    echo [ERROR] MSBuild not found
    echo Please install Visual Studio Build Tools or Visual Studio
    pause
    exit /b 1
)

REM Create build directory
if not exist "build-quick" mkdir "build-quick"

echo [INFO] Configuring CMake (this may take a while on first run)...
cmake -B build-quick -G "Visual Studio 17 2022" -A x64

if errorlevel 1 (
    echo [ERROR] CMake configuration failed
    echo.
    echo Common solutions:
    echo 1. Install missing dependencies with vcpkg:
    echo    vcpkg install glfw3:x64-windows glew:x64-windows openal-soft:x64-windows
    echo 2. Make sure Visual Studio 2022 is installed
    echo 3. Check that all source files exist
    pause
    exit /b 1
)

echo [INFO] Building project...
cmake --build build-quick --config Release --parallel

if errorlevel 1 (
    echo [ERROR] Build failed
    echo Check the output above for specific compilation errors
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Build completed successfully!
echo.
echo Executable: build-quick\Release\simple_shooter.exe
echo Shared library: build-quick\Release\game_core.dll
echo.

REM Test if executable runs
if exist "build-quick\Release\simple_shooter.exe" (
    echo [INFO] Testing executable...
    cd build-quick\Release
    echo Trying to run simple_shooter.exe --help...
    simple_shooter.exe --help 2>nul
    if errorlevel 1 (
        echo [WARNING] Executable test failed, but build was successful
    ) else (
        echo [SUCCESS] Executable runs correctly!
    )
    cd ..\..
)

echo.
echo [INFO] Build artifacts are in build-quick\Release\
echo To run the game: cd build-quick\Release && simple_shooter.exe
echo.
pause