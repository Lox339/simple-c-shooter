# Simple C Shooter

[![Build Status](https://github.com/username/simple-c-shooter/workflows/Build%20Simple%20C%20Shooter/badge.svg)](https://github.com/username/simple-c-shooter/actions)
[![Test Status](https://github.com/username/simple-c-shooter/workflows/Test%20Simple%20C%20Shooter/badge.svg)](https://github.com/username/simple-c-shooter/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A modern 3D shooter game featuring advanced bunny hop mechanics, built with a multi-language architecture combining C, C++, and C#.

## 🎮 Features

### Core Gameplay
- **Advanced Bunny Hop Mechanics** - Chain jumps to gain incredible speed
- **Real-time Speedometer** - Track your velocity with color-coded display
- **Smart AI Enemies** - Three enemy types with different behaviors
- **3D Positional Audio** - Immersive sound experience with Doppler effects
- **Collision System** - Precise hit detection with visual effects

### Technical Features
- **Multi-language Architecture** - C core engine, C++ graphics/physics, C# UI
- **Cross-platform Support** - Windows, Linux, and macOS
- **Modern OpenGL Rendering** - 3D graphics with lighting and effects
- **Professional UI System** - Complete menu system with settings
- **CI/CD Pipeline** - Automated building and testing

## 🚀 Quick Start

### Prerequisites

#### Windows
- Visual Studio 2019/2022 with C++ support
- .NET 6.0 SDK
- vcpkg for dependencies

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install build-essential cmake libglfw3-dev libglew-dev libgl1-mesa-dev libopenal-dev libsndfile1-dev pkg-config
```

#### macOS
```bash
brew install cmake glfw glew openal-soft libsndfile pkg-config
```

### Building

1. **Clone the repository**
```bash
git clone https://github.com/username/simple-c-shooter.git
cd simple-c-shooter
```

2. **Build C/C++ components**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

3. **Build C# UI components**
```bash
dotnet restore src/ui/SimpleShooter.UI.csproj
dotnet build src/ui/SimpleShooter.UI.csproj --configuration Release
```

4. **Run the game**
```bash
./simple_shooter
```

## 🎯 Controls

| Key/Action | Function |
|------------|----------|
| **WASD** | Move player |
| **Mouse** | Look around |
| **Space** | Jump (chain for bunny hopping) |
| **Left Click** | Shoot |
| **ESC** | Pause/Resume game |
| **Q** | Quit game |
| **O** | Open audio settings |

### Bunny Hop Tips
1. **Strafe while jumping** - Use A/D keys while in air
2. **Don't hold W** - Forward key reduces air acceleration
3. **Smooth mouse movement** - Gentle curves maintain speed
4. **Chain jumps** - Land and immediately jump again
5. **Watch the speedometer** - Red color means you're flying!

## 🏗️ Architecture

### Multi-Language Design
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   C# UI Layer   │    │  C++ Graphics   │    │   C Core Engine │
│                 │    │   & Physics     │    │                 │
│ • Menus         │◄──►│ • OpenGL        │◄──►│ • Game Logic    │
│ • Settings      │    │ • 3D Rendering  │    │ • Input         │
│ • HUD           │    │ • Bunny Hop     │    │ • AI System     │
│ • Speedometer   │    │ • Audio System  │    │ • Collisions    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Component Overview

#### Core Engine (C)
- **Game Loop** - Main game loop with FPS control
- **Input Manager** - Cross-platform input handling
- **Object Manager** - Entity management system
- **Enemy AI** - Intelligent enemy behaviors
- **Collision System** - Precise collision detection

#### Graphics & Physics (C++)
- **Renderer** - Modern OpenGL 3.3+ rendering
- **Camera System** - First-person camera with smooth movement
- **Physics Engine** - Custom physics with bunny hop mechanics
- **Particle Effects** - Visual effects for impacts and trails
- **Audio System** - 3D positional audio with OpenAL

#### UI System (C#)
- **Main Menu** - Professional game menu
- **Settings** - Comprehensive options menu
- **HUD** - In-game interface with health, ammo, score
- **Speedometer** - Real-time speed display with color coding

## 🎨 Game Mechanics

### Bunny Hopping
The game features an authentic bunny hop system inspired by classic FPS games:

- **Air Strafing** - Gain speed by strafing while airborne
- **Speed Preservation** - Maintain momentum between jumps
- **No Speed Cap** - Theoretical unlimited speed potential
- **Visual Feedback** - Speedometer changes color with velocity

### Enemy Types
- **Basic Enemy** (Red Cube) - Balanced health and speed
- **Fast Enemy** (Green Sphere) - Quick but fragile
- **Heavy Enemy** (Blue Cube) - Slow but heavily armored

### Audio System
- **3D Positional Audio** - Sounds positioned in 3D space
- **Doppler Effects** - Pitch changes based on movement
- **Dynamic Footsteps** - Automatic footstep sounds
- **Environmental Audio** - Bunny hop landing sounds

## 🛠️ Development

### Project Structure
```
simple-c-shooter/
├── src/
│   ├── core/           # C core engine
│   ├── graphics/       # C++ graphics engine
│   ├── physics/        # C++ physics engine
│   ├── audio/          # C audio system
│   ├── ui/             # C# UI components
│   └── main.c          # Entry point
├── include/            # Shared headers
├── .github/workflows/  # CI/CD pipelines
├── CMakeLists.txt      # Build configuration
└── README.md
```

### Building from Source

#### Debug Build
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

#### Release Build
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

#### Running Tests
```bash
cd build
ctest --output-on-failure
```

### Command Line Options
```bash
./simple_shooter --help                 # Show help
./simple_shooter --version              # Show version
./simple_shooter --test                 # Test mode
./simple_shooter --fps 120              # Set target FPS
./simple_shooter --windowed             # Force windowed mode
./simple_shooter --no-audio             # Disable audio
./simple_shooter --debug                # Enable debug output
```

## 🧪 Testing

The project includes comprehensive testing:

- **Unit Tests** - Core functionality testing
- **Integration Tests** - Multi-language component testing
- **Performance Tests** - FPS and memory usage validation
- **Compatibility Tests** - Cross-platform verification
- **Static Analysis** - Code quality checks
- **Memory Leak Detection** - Valgrind integration

Run all tests:
```bash
# Build and run tests
cmake --build build --target test

# Run with memory checking (Linux)
valgrind --tool=memcheck build/simple_shooter --test
```

## 📦 Releases

Pre-built binaries are available for:
- Windows (x64)
- Linux (x64)
- macOS (x64)

Download from the [Releases](https://github.com/username/simple-c-shooter/releases) page.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Guidelines
- Follow the existing code style
- Add tests for new features
- Update documentation as needed
- Ensure CI/CD passes

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Quake Engine** - Inspiration for bunny hop mechanics
- **OpenGL Community** - Graphics programming resources
- **GLFW & GLEW** - Essential OpenGL libraries
- **OpenAL** - 3D audio capabilities

## 📊 Performance

### System Requirements

#### Minimum
- **OS**: Windows 10, Ubuntu 18.04, macOS 10.14
- **CPU**: Dual-core 2.0 GHz
- **Memory**: 2 GB RAM
- **Graphics**: OpenGL 3.3 compatible
- **Storage**: 100 MB available space

#### Recommended
- **OS**: Windows 11, Ubuntu 22.04, macOS 12.0
- **CPU**: Quad-core 3.0 GHz
- **Memory**: 4 GB RAM
- **Graphics**: Dedicated GPU with OpenGL 4.0+
- **Storage**: 500 MB available space

### Performance Targets
- **60+ FPS** at 1080p on recommended hardware
- **<100ms** input latency
- **<50MB** memory usage
- **<5s** startup time

## 🐛 Known Issues

- Audio may not work on some Linux distributions without additional packages
- High DPI displays may have scaling issues
- Some older Intel integrated graphics may have rendering artifacts

## 🔮 Future Plans

- [ ] Multiplayer support
- [ ] Level editor
- [ ] Steam Workshop integration
- [ ] VR support
- [ ] Mobile port (Android/iOS)
- [ ] Mod support

---

**Made with ❤️ by the Simple Shooter Team**

*Experience the thrill of bunny hopping in a modern 3D shooter!*