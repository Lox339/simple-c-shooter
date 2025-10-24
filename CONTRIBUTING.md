# Contributing to Simple C Shooter

Thank you for your interest in contributing to Simple C Shooter! This document provides guidelines and information for contributors.

## 🚀 Getting Started

### Prerequisites
- Git
- CMake 3.20+
- C/C++ compiler (GCC, Clang, or MSVC)
- .NET 6.0 SDK
- Platform-specific dependencies (see README.md)

### Setting up Development Environment

1. **Fork and clone the repository**
```bash
git clone https://github.com/your-username/simple-c-shooter.git
cd simple-c-shooter
```

2. **Create a development branch**
```bash
git checkout -b feature/your-feature-name
```

3. **Build the project**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel
```

4. **Run tests**
```bash
ctest --output-on-failure
```

## 📋 How to Contribute

### Reporting Bugs
1. Check existing issues to avoid duplicates
2. Use the bug report template
3. Include system information and steps to reproduce
4. Attach logs or screenshots if relevant

### Suggesting Features
1. Check existing feature requests
2. Use the feature request template
3. Explain the use case and expected behavior
4. Consider implementation complexity

### Code Contributions

#### Types of Contributions Welcome
- **Bug fixes** - Fix existing issues
- **Performance improvements** - Optimize existing code
- **New features** - Add new gameplay mechanics
- **Documentation** - Improve docs and comments
- **Tests** - Add or improve test coverage
- **Platform support** - Add support for new platforms

#### Before You Start
1. Check existing issues and PRs
2. Discuss major changes in an issue first
3. Follow the coding standards
4. Ensure your changes don't break existing functionality

## 🎯 Development Guidelines

### Code Style

#### C Code Style
```c
// Use snake_case for functions and variables
void update_game_state(float delta_time);
int enemy_count = 0;

// Use UPPER_CASE for constants
#define MAX_ENEMIES 50
#define GAME_VERSION "1.0.0"

// Function documentation
/**
 * Updates the game state for one frame
 * @param delta_time Time elapsed since last frame in seconds
 */
void update_game_state(float delta_time);
```

#### C++ Code Style
```cpp
// Use PascalCase for classes, camelCase for methods
class EnemyAI {
public:
    void updateBehavior(float deltaTime);
    
private:
    float detectionRange;
    AIState currentState;
};

// Use descriptive names
std::vector<Enemy> activeEnemies;
```

#### C# Code Style
```csharp
// Follow Microsoft C# conventions
public class GameHUD
{
    public void UpdateDisplay(PlayerState player)
    {
        // Use PascalCase for public members
        this.HealthBar.UpdateValue(player.Health);
    }
    
    private float healthBarWidth; // camelCase for private fields
}
```

### Architecture Guidelines

#### Multi-Language Integration
- **C Core**: Game logic, input, AI, basic physics
- **C++ Graphics/Physics**: Rendering, advanced physics, audio
- **C# UI**: Menus, HUD, settings, user interface

#### Communication Between Languages
- Use C-compatible interfaces for all inter-language calls
- Minimize data marshaling overhead
- Handle errors gracefully across language boundaries

#### Performance Considerations
- Target 60+ FPS on recommended hardware
- Minimize memory allocations in game loop
- Use object pooling for frequently created/destroyed objects
- Profile performance-critical code paths

### Testing Guidelines

#### Unit Tests
- Test individual functions and classes
- Mock external dependencies
- Aim for >80% code coverage on core logic

#### Integration Tests
- Test interaction between components
- Verify C/C++/C# interoperability
- Test complete game scenarios

#### Performance Tests
- Benchmark critical code paths
- Monitor memory usage
- Verify frame rate targets

### Commit Guidelines

#### Commit Message Format
```
type(scope): brief description

Detailed explanation of changes (if needed)

Fixes #123
```

#### Types
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `test`: Adding or updating tests
- `chore`: Build process or auxiliary tool changes

#### Examples
```
feat(physics): add bunny hop speed limiting

Add configurable maximum speed for bunny hop mechanics
to prevent physics engine instability at extreme velocities.

Fixes #45
```

```
fix(audio): resolve memory leak in sound system

Free allocated sound buffers when audio sources are destroyed.
Detected by Valgrind in CI pipeline.

Fixes #67
```

## 🔄 Pull Request Process

### Before Submitting
1. **Update your branch**
```bash
git checkout main
git pull upstream main
git checkout your-feature-branch
git rebase main
```

2. **Run all tests**
```bash
cmake --build build --target test
```

3. **Check code quality**
```bash
# Run static analysis
cppcheck src/
clang-tidy src/**/*.cpp

# Check formatting
clang-format -i src/**/*.{c,cpp,h,hpp}
```

### Pull Request Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review completed
- [ ] Tests added/updated for changes
- [ ] Documentation updated if needed
- [ ] No breaking changes (or clearly documented)
- [ ] CI/CD pipeline passes
- [ ] Linked to relevant issues

### Review Process
1. **Automated checks** - CI/CD must pass
2. **Code review** - At least one maintainer approval
3. **Testing** - Manual testing if needed
4. **Merge** - Squash and merge preferred

## 🏗️ Project Structure

### Directory Layout
```
simple-c-shooter/
├── src/
│   ├── core/           # C core engine
│   │   ├── game_loop.c
│   │   ├── input_manager.c
│   │   └── ...
│   ├── graphics/       # C++ graphics
│   │   ├── renderer.cpp
│   │   ├── camera.cpp
│   │   └── ...
│   ├── physics/        # C++ physics
│   │   ├── physics_engine.cpp
│   │   ├── bunny_hop.cpp
│   │   └── ...
│   ├── audio/          # C audio system
│   │   ├── audio_system.c
│   │   └── ...
│   └── ui/             # C# UI components
│       ├── UIManager.cs
│       ├── GameHUD.cs
│       └── ...
├── include/            # Shared headers
├── .github/            # CI/CD workflows
├── docs/               # Documentation
└── tests/              # Test files
```

### Key Components
- **Game Loop** - Main game loop and timing
- **Input System** - Cross-platform input handling
- **Physics Engine** - Custom physics with bunny hop
- **Graphics Engine** - OpenGL rendering pipeline
- **Audio System** - 3D positional audio
- **UI System** - Game interface and menus
- **AI System** - Enemy behaviors and pathfinding

## 🧪 Testing Your Changes

### Local Testing
```bash
# Build debug version
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Run basic tests
cd build && ctest

# Run game in test mode
./simple_shooter --test

# Run with memory checking (Linux)
valgrind --tool=memcheck ./simple_shooter --test
```

### Platform Testing
Test your changes on multiple platforms if possible:
- Windows (Visual Studio)
- Linux (GCC/Clang)
- macOS (Clang)

## 📚 Resources

### Documentation
- [OpenGL Documentation](https://www.opengl.org/documentation/)
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [OpenAL Documentation](https://openal.org/documentation/)
- [.NET Documentation](https://docs.microsoft.com/en-us/dotnet/)

### Learning Resources
- [Bunny Hopping Physics](https://adrianb.io/2015/02/14/bunnyhop.html)
- [Game Engine Architecture](https://www.gameenginebook.com/)
- [Real-Time Rendering](http://www.realtimerendering.com/)

### Tools
- **Profiling**: Valgrind, Visual Studio Profiler, Intel VTune
- **Debugging**: GDB, Visual Studio Debugger, LLDB
- **Static Analysis**: Cppcheck, Clang Static Analyzer, PVS-Studio

## 🤝 Community

### Communication
- **GitHub Issues** - Bug reports and feature requests
- **GitHub Discussions** - General questions and ideas
- **Pull Requests** - Code review and collaboration

### Code of Conduct
- Be respectful and inclusive
- Focus on constructive feedback
- Help newcomers learn and contribute
- Follow GitHub's Community Guidelines

## 🎖️ Recognition

Contributors will be recognized in:
- README.md contributors section
- Release notes for significant contributions
- Special thanks for major features or fixes

## ❓ Questions?

If you have questions about contributing:
1. Check existing documentation
2. Search GitHub issues and discussions
3. Create a new discussion or issue
4. Tag maintainers if urgent

Thank you for contributing to Simple C Shooter! 🎮