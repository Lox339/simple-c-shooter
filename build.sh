#!/bin/bash

# Simple C Shooter - Build Script
# Cross-platform build script for Unix-like systems

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
BUILD_DIR="build"
CLEAN_BUILD=false
RUN_TESTS=false
INSTALL=false
VERBOSE=false
PARALLEL_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to show usage
show_usage() {
    cat << EOF
Simple C Shooter Build Script

Usage: $0 [OPTIONS]

OPTIONS:
    -h, --help          Show this help message
    -d, --debug         Build in Debug mode (default: Release)
    -c, --clean         Clean build directory before building
    -t, --test          Run tests after building
    -i, --install       Install after building
    -v, --verbose       Verbose output
    -j, --jobs N        Number of parallel jobs (default: $PARALLEL_JOBS)
    --build-dir DIR     Build directory (default: $BUILD_DIR)

EXAMPLES:
    $0                  # Release build
    $0 -d -t            # Debug build with tests
    $0 -c -i            # Clean release build with install
    $0 --debug --clean --test --verbose  # Full debug build

ENVIRONMENT VARIABLES:
    CC                  C compiler (default: auto-detect)
    CXX                 C++ compiler (default: auto-detect)
    CMAKE_ARGS          Additional CMake arguments
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_usage
            exit 0
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -t|--test)
            RUN_TESTS=true
            shift
            ;;
        -i|--install)
            INSTALL=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

# Print build configuration
print_status "Simple C Shooter Build Configuration"
echo "  Build Type: $BUILD_TYPE"
echo "  Build Directory: $BUILD_DIR"
echo "  Parallel Jobs: $PARALLEL_JOBS"
echo "  Clean Build: $CLEAN_BUILD"
echo "  Run Tests: $RUN_TESTS"
echo "  Install: $INSTALL"
echo "  Verbose: $VERBOSE"
echo ""

# Check for required tools
print_status "Checking build requirements..."

# Check for CMake
if ! command -v cmake &> /dev/null; then
    print_error "CMake is required but not installed"
    exit 1
fi

CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
print_success "CMake found: $CMAKE_VERSION"

# Check for compilers
if [[ -n "$CC" ]]; then
    print_status "Using C compiler: $CC"
else
    if command -v gcc &> /dev/null; then
        export CC=gcc
        print_success "C compiler found: gcc"
    elif command -v clang &> /dev/null; then
        export CC=clang
        print_success "C compiler found: clang"
    else
        print_error "No C compiler found (gcc or clang required)"
        exit 1
    fi
fi

if [[ -n "$CXX" ]]; then
    print_status "Using C++ compiler: $CXX"
else
    if command -v g++ &> /dev/null; then
        export CXX=g++
        print_success "C++ compiler found: g++"
    elif command -v clang++ &> /dev/null; then
        export CXX=clang++
        print_success "C++ compiler found: clang++"
    else
        print_error "No C++ compiler found (g++ or clang++ required)"
        exit 1
    fi
fi

# Check for .NET (for C# components)
if command -v dotnet &> /dev/null; then
    DOTNET_VERSION=$(dotnet --version)
    print_success ".NET found: $DOTNET_VERSION"
else
    print_warning ".NET not found - C# UI components will not be built"
fi

# Clean build directory if requested
if [[ "$CLEAN_BUILD" == true ]]; then
    print_status "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    print_success "Build directory cleaned"
fi

# Create build directory
print_status "Creating build directory..."
mkdir -p "$BUILD_DIR"

# Configure CMake
print_status "Configuring CMake..."
CMAKE_CMD="cmake -B $BUILD_DIR -DCMAKE_BUILD_TYPE=$BUILD_TYPE"

# Add verbose flag if requested
if [[ "$VERBOSE" == true ]]; then
    CMAKE_CMD="$CMAKE_CMD -DCMAKE_VERBOSE_MAKEFILE=ON"
fi

# Add any additional CMake arguments
if [[ -n "$CMAKE_ARGS" ]]; then
    CMAKE_CMD="$CMAKE_CMD $CMAKE_ARGS"
fi

print_status "Running: $CMAKE_CMD"
eval $CMAKE_CMD

if [[ $? -eq 0 ]]; then
    print_success "CMake configuration completed"
else
    print_error "CMake configuration failed"
    exit 1
fi

# Build the project
print_status "Building project..."
BUILD_CMD="cmake --build $BUILD_DIR --config $BUILD_TYPE --parallel $PARALLEL_JOBS"

if [[ "$VERBOSE" == true ]]; then
    BUILD_CMD="$BUILD_CMD --verbose"
fi

print_status "Running: $BUILD_CMD"
eval $BUILD_CMD

if [[ $? -eq 0 ]]; then
    print_success "Build completed successfully"
else
    print_error "Build failed"
    exit 1
fi

# Build C# components if .NET is available
if command -v dotnet &> /dev/null; then
    print_status "Building C# UI components..."
    
    if [[ -f "src/ui/SimpleShooter.UI.csproj" ]]; then
        dotnet restore src/ui/SimpleShooter.UI.csproj
        dotnet build src/ui/SimpleShooter.UI.csproj --configuration $BUILD_TYPE --no-restore
        
        if [[ $? -eq 0 ]]; then
            print_success "C# components built successfully"
        else
            print_warning "C# components build failed"
        fi
    else
        print_warning "C# project file not found"
    fi
fi

# Run tests if requested
if [[ "$RUN_TESTS" == true ]]; then
    print_status "Running tests..."
    
    cd "$BUILD_DIR"
    ctest --output-on-failure --parallel $PARALLEL_JOBS
    
    if [[ $? -eq 0 ]]; then
        print_success "All tests passed"
    else
        print_error "Some tests failed"
        cd ..
        exit 1
    fi
    cd ..
fi

# Install if requested
if [[ "$INSTALL" == true ]]; then
    print_status "Installing..."
    
    cmake --install "$BUILD_DIR" --config $BUILD_TYPE
    
    if [[ $? -eq 0 ]]; then
        print_success "Installation completed"
    else
        print_error "Installation failed"
        exit 1
    fi
fi

# Print final status
echo ""
print_success "Build process completed successfully!"
echo ""
print_status "Build artifacts:"
echo "  Executable: $BUILD_DIR/simple_shooter"
echo "  Shared library: $BUILD_DIR/libgame_core.so (or .dll/.dylib)"
echo ""
print_status "To run the game:"
echo "  cd $BUILD_DIR && ./simple_shooter"
echo ""
print_status "For help:"
echo "  cd $BUILD_DIR && ./simple_shooter --help"