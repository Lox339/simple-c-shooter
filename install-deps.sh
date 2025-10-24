#!/bin/bash

# Simple C Shooter - Dependency Installation Script
# Automatically installs required dependencies for different platforms

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

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

# Detect OS
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt-get &> /dev/null; then
            OS="ubuntu"
        elif command -v yum &> /dev/null; then
            OS="centos"
        elif command -v pacman &> /dev/null; then
            OS="arch"
        else
            OS="linux"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    else
        OS="unknown"
    fi
}

# Install dependencies for Ubuntu/Debian
install_ubuntu_deps() {
    print_status "Installing dependencies for Ubuntu/Debian..."
    
    sudo apt-get update
    sudo apt-get install -y \
        build-essential \
        cmake \
        git \
        pkg-config \
        libglfw3-dev \
        libglew-dev \
        libgl1-mesa-dev \
        libopenal-dev \
        libsndfile1-dev \
        wget \
        curl
    
    print_success "Ubuntu/Debian dependencies installed"
}

# Install dependencies for CentOS/RHEL/Fedora
install_centos_deps() {
    print_status "Installing dependencies for CentOS/RHEL/Fedora..."
    
    if command -v dnf &> /dev/null; then
        PKG_MANAGER="dnf"
    else
        PKG_MANAGER="yum"
    fi
    
    sudo $PKG_MANAGER install -y \
        gcc \
        gcc-c++ \
        cmake \
        git \
        pkgconfig \
        glfw-devel \
        glew-devel \
        mesa-libGL-devel \
        openal-soft-devel \
        libsndfile-devel \
        wget \
        curl
    
    print_success "CentOS/RHEL/Fedora dependencies installed"
}

# Install dependencies for Arch Linux
install_arch_deps() {
    print_status "Installing dependencies for Arch Linux..."
    
    sudo pacman -S --needed \
        base-devel \
        cmake \
        git \
        pkgconf \
        glfw-x11 \
        glew \
        mesa \
        openal \
        libsndfile \
        wget \
        curl
    
    print_success "Arch Linux dependencies installed"
}

# Install dependencies for macOS
install_macos_deps() {
    print_status "Installing dependencies for macOS..."
    
    # Check if Homebrew is installed
    if ! command -v brew &> /dev/null; then
        print_status "Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    # Update Homebrew
    brew update
    
    # Install dependencies
    brew install \
        cmake \
        git \
        pkg-config \
        glfw \
        glew \
        openal-soft \
        libsndfile
    
    print_success "macOS dependencies installed"
}

# Install .NET SDK
install_dotnet() {
    print_status "Installing .NET SDK..."
    
    case $OS in
        ubuntu)
            # Add Microsoft package repository
            wget https://packages.microsoft.com/config/ubuntu/$(lsb_release -rs)/packages-microsoft-prod.deb -O packages-microsoft-prod.deb
            sudo dpkg -i packages-microsoft-prod.deb
            rm packages-microsoft-prod.deb
            
            sudo apt-get update
            sudo apt-get install -y dotnet-sdk-6.0
            ;;
        centos)
            sudo rpm -Uvh https://packages.microsoft.com/config/centos/7/packages-microsoft-prod.rpm
            sudo $PKG_MANAGER install -y dotnet-sdk-6.0
            ;;
        arch)
            sudo pacman -S --needed dotnet-sdk
            ;;
        macos)
            brew install --cask dotnet
            ;;
        *)
            print_warning "Please install .NET SDK manually from https://dotnet.microsoft.com/download"
            return
            ;;
    esac
    
    print_success ".NET SDK installed"
}

# Verify installations
verify_installation() {
    print_status "Verifying installations..."
    
    # Check CMake
    if command -v cmake &> /dev/null; then
        CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
        print_success "CMake: $CMAKE_VERSION"
    else
        print_error "CMake not found"
        return 1
    fi
    
    # Check compilers
    if command -v gcc &> /dev/null; then
        GCC_VERSION=$(gcc --version | head -n1)
        print_success "GCC: $GCC_VERSION"
    elif command -v clang &> /dev/null; then
        CLANG_VERSION=$(clang --version | head -n1)
        print_success "Clang: $CLANG_VERSION"
    else
        print_error "No C compiler found"
        return 1
    fi
    
    # Check .NET
    if command -v dotnet &> /dev/null; then
        DOTNET_VERSION=$(dotnet --version)
        print_success ".NET SDK: $DOTNET_VERSION"
    else
        print_warning ".NET SDK not found"
    fi
    
    # Check pkg-config
    if command -v pkg-config &> /dev/null; then
        print_success "pkg-config: Available"
    else
        print_warning "pkg-config not found"
    fi
    
    print_success "Verification completed"
}

# Main installation function
main() {
    print_status "Simple C Shooter - Dependency Installation"
    echo ""
    
    detect_os
    print_status "Detected OS: $OS"
    
    case $OS in
        ubuntu)
            install_ubuntu_deps
            ;;
        centos)
            install_centos_deps
            ;;
        arch)
            install_arch_deps
            ;;
        macos)
            install_macos_deps
            ;;
        *)
            print_error "Unsupported operating system: $OSTYPE"
            print_status "Please install dependencies manually:"
            echo "  - CMake 3.20+"
            echo "  - C/C++ compiler (GCC or Clang)"
            echo "  - GLFW 3.x"
            echo "  - GLEW"
            echo "  - OpenGL"
            echo "  - OpenAL"
            echo "  - libsndfile"
            echo "  - .NET 6.0 SDK"
            exit 1
            ;;
    esac
    
    # Install .NET SDK
    if [[ "$1" != "--no-dotnet" ]]; then
        install_dotnet
    fi
    
    # Verify installation
    verify_installation
    
    echo ""
    print_success "All dependencies installed successfully!"
    print_status "You can now build the project with:"
    echo "  ./build.sh"
    echo ""
    print_status "Or build manually with:"
    echo "  mkdir build && cd build"
    echo "  cmake .. -DCMAKE_BUILD_TYPE=Release"
    echo "  cmake --build . --parallel"
}

# Show usage if help requested
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    cat << EOF
Simple C Shooter - Dependency Installation Script

Usage: $0 [OPTIONS]

OPTIONS:
    --help, -h      Show this help message
    --no-dotnet     Skip .NET SDK installation

This script automatically installs the required dependencies for building
Simple C Shooter on supported platforms:

  - Ubuntu/Debian (apt)
  - CentOS/RHEL/Fedora (yum/dnf)
  - Arch Linux (pacman)
  - macOS (Homebrew)

Dependencies installed:
  - CMake 3.20+
  - C/C++ compiler (GCC or Clang)
  - GLFW 3.x (OpenGL window management)
  - GLEW (OpenGL extension loading)
  - OpenGL (graphics)
  - OpenAL (3D audio)
  - libsndfile (audio file loading)
  - .NET 6.0 SDK (C# UI components)

EOF
    exit 0
fi

# Run main function
main "$@"