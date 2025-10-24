@echo off
REM Simple C Shooter - Windows Build Script
REM Cross-platform build script for Windows

setlocal enabledelayedexpansion

REM Default values
set BUILD_TYPE=Release
set BUILD_DIR=build
set CLEAN_BUILD=false
set RUN_TESTS=false
set INSTALL=false
set VERBOSE=false
set PARALLEL_JOBS=%NUMBER_OF_PROCESSORS%

REM Colors (if supported)
set RED=[91m
set GREEN=[92m
set YELLOW=[93m
set BLUE=[94m
set NC=[0m

REM Function to print status (simulated with goto)
goto :parse_args

:print_status
echo [INFO] %~1
goto :eof

:print_success
echo [SUCCESS] %~1
goto :eof

:print_warning
echo [WARNING] %~1
goto :eof

:print_error
echo [ERROR] %~1
goto :eof

:show_usage
echo Simple C Shooter Build Script for Windows
echo.
echo Usage: %0 [OPTIONS]
echo.
echo OPTIONS:
echo     /h, /help          Show this help message
echo     /d, /debug         Build in Debug mode (default: Release)
echo     /c, /clean         Clean build directory before building
echo     /t, /test          Run tests after building
echo     /i, /install       Install after building
echo     /v, /verbose       Verbose output
echo     /j:N               Number of parallel jobs (default: %NUMBER_OF_PROCESSORS%)
echo     /build-dir:DIR     Build directory (default: %BUILD_DIR%)
echo.
echo EXAMPLES:
echo     %0                 # Release build
echo     %0 /d /t           # Debug build with tests
echo     %0 /c /i           # Clean release build with install
echo.
echo ENVIRONMENT VARIABLES:
echo     CMAKE_ARGS         Additional CMake arguments
goto :eof

:parse_args
if "%~1"=="" goto :start_build
if /i "%~1"=="/h" goto :show_usage_and_exit
if /i "%~1"=="/help" goto :show_usage_and_exit
if /i "%~1"=="/d" (
    set BUILD_TYPE=Debug
    shift
    goto :parse_args
)
if /i "%~1"=="/debug" (
    set BUILD_TYPE=Debug
    shift
    goto :parse_args
)
if /i "%~1"=="/c" (
    set CLEAN_BUILD=true
    shift
    goto :parse_args
)
if /i "%~1"=="/clean" (
    set CLEAN_BUILD=true
    shift
    goto :parse_args
)
if /i "%~1"=="/t" (
    set RUN_TESTS=true
    shift
    goto :parse_args
)
if /i "%~1"=="/test" (
    set RUN_TESTS=true
    shift
    goto :parse_args
)
if /i "%~1"=="/i" (
    set INSTALL=true
    shift
    goto :parse_args
)
if /i "%~1"=="/install" (
    set INSTALL=true
    shift
    goto :parse_args
)
if /i "%~1"=="/v" (
    set VERBOSE=true
    shift
    goto :parse_args
)
if /i "%~1"=="/verbose" (
    set VERBOSE=true
    shift
    goto :parse_args
)
if "%~1" NEQ "" (
    if "%~1:~0,3%"=="/j:" (
        set PARALLEL_JOBS=%~1:~3%
        shift
        goto :parse_args
    )
    if "%~1:~0,11%"=="/build-dir:" (
        set BUILD_DIR=%~1:~11%
        shift
        goto :parse_args
    )
    call :print_error "Unknown option: %~1"
    goto :show_usage_and_exit
)
shift
goto :parse_args

:show_usage_and_exit
call :show_usage
exit /b 1

:start_build
REM Print build configuration
call :print_status "Simple C Shooter Build Configuration"
echo   Build Type: %BUILD_TYPE%
echo   Build Directory: %BUILD_DIR%
echo   Parallel Jobs: %PARALLEL_JOBS%
echo   Clean Build: %CLEAN_BUILD%
echo   Run Tests: %RUN_TESTS%
echo   Install: %INSTALL%
echo   Verbose: %VERBOSE%
echo.

REM Check for required tools
call :print_status "Checking build requirements..."

REM Check for CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    call :print_error "CMake is required but not installed"
    exit /b 1
)

for /f "tokens=3" %%i in ('cmake --version ^| findstr /r "cmake version"') do set CMAKE_VERSION=%%i
call :print_success "CMake found: %CMAKE_VERSION%"

REM Check for Visual Studio Build Tools
where msbuild >nul 2>&1
if errorlevel 1 (
    call :print_error "MSBuild not found. Please install Visual Studio Build Tools"
    exit /b 1
)
call :print_success "MSBuild found"

REM Check for .NET
dotnet --version >nul 2>&1
if errorlevel 1 (
    call :print_warning ".NET not found - C# UI components will not be built"
) else (
    for /f %%i in ('dotnet --version') do set DOTNET_VERSION=%%i
    call :print_success ".NET found: !DOTNET_VERSION!"
)

REM Check for vcpkg (optional)
where vcpkg >nul 2>&1
if errorlevel 1 (
    call :print_warning "vcpkg not found - dependencies must be installed manually"
) else (
    call :print_success "vcpkg found"
    set CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
)

REM Clean build directory if requested
if "%CLEAN_BUILD%"=="true" (
    call :print_status "Cleaning build directory..."
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
    call :print_success "Build directory cleaned"
)

REM Create build directory
call :print_status "Creating build directory..."
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Configure CMake
call :print_status "Configuring CMake..."
set CMAKE_CMD=cmake -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -G "Visual Studio 17 2022" -A x64

REM Add verbose flag if requested
if "%VERBOSE%"=="true" (
    set CMAKE_CMD=%CMAKE_CMD% -DCMAKE_VERBOSE_MAKEFILE=ON
)

REM Add any additional CMake arguments
if defined CMAKE_ARGS (
    set CMAKE_CMD=%CMAKE_CMD% %CMAKE_ARGS%
)

call :print_status "Running: %CMAKE_CMD%"
%CMAKE_CMD%

if errorlevel 1 (
    call :print_error "CMake configuration failed"
    exit /b 1
)
call :print_success "CMake configuration completed"

REM Build the project
call :print_status "Building project..."
set BUILD_CMD=cmake --build %BUILD_DIR% --config %BUILD_TYPE% --parallel %PARALLEL_JOBS%

if "%VERBOSE%"=="true" (
    set BUILD_CMD=%BUILD_CMD% --verbose
)

call :print_status "Running: %BUILD_CMD%"
%BUILD_CMD%

if errorlevel 1 (
    call :print_error "Build failed"
    exit /b 1
)
call :print_success "Build completed successfully"

REM Build C# components if .NET is available
dotnet --version >nul 2>&1
if not errorlevel 1 (
    call :print_status "Building C# UI components..."
    
    if exist "src\ui\SimpleShooter.UI.csproj" (
        dotnet restore src\ui\SimpleShooter.UI.csproj
        dotnet build src\ui\SimpleShooter.UI.csproj --configuration %BUILD_TYPE% --no-restore
        
        if errorlevel 1 (
            call :print_warning "C# components build failed"
        ) else (
            call :print_success "C# components built successfully"
        )
    ) else (
        call :print_warning "C# project file not found"
    )
)

REM Run tests if requested
if "%RUN_TESTS%"=="true" (
    call :print_status "Running tests..."
    
    cd /d "%BUILD_DIR%"
    ctest --output-on-failure --parallel %PARALLEL_JOBS% -C %BUILD_TYPE%
    
    if errorlevel 1 (
        call :print_error "Some tests failed"
        cd /d ..
        exit /b 1
    )
    call :print_success "All tests passed"
    cd /d ..
)

REM Install if requested
if "%INSTALL%"=="true" (
    call :print_status "Installing..."
    
    cmake --install "%BUILD_DIR%" --config %BUILD_TYPE%
    
    if errorlevel 1 (
        call :print_error "Installation failed"
        exit /b 1
    )
    call :print_success "Installation completed"
)

REM Print final status
echo.
call :print_success "Build process completed successfully!"
echo.
call :print_status "Build artifacts:"
echo   Executable: %BUILD_DIR%\%BUILD_TYPE%\simple_shooter.exe
echo   Shared library: %BUILD_DIR%\%BUILD_TYPE%\game_core.dll
echo.
call :print_status "To run the game:"
echo   cd %BUILD_DIR%\%BUILD_TYPE% ^&^& simple_shooter.exe
echo.
call :print_status "For help:"
echo   cd %BUILD_DIR%\%BUILD_TYPE% ^&^& simple_shooter.exe --help

endlocal