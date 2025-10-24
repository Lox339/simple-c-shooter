# Simple 3D Shooter

[![Build Status](https://github.com/Lox339/simple-c-shooter/workflows/Build%20Multi-Language%203D%20Shooter/badge.svg)](https://github.com/Lox339/simple-c-shooter/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C](https://img.shields.io/badge/C-11-blue.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![C#](https://img.shields.io/badge/C%23-6.0-purple.svg)](https://docs.microsoft.com/en-us/dotnet/csharp/)

3D шутер от первого лица, разработанный с использованием трех языков программирования:
- **C** - основной игровой движок
- **C++** - графическая подсистема и физика  
- **C#** - пользовательский интерфейс

## Особенности

- 3D графика от первого лица
- Банихоп механика для продвинутого движения
- Спидометр в реальном времени
- Межъязыковое взаимодействие через C API
- Автоматическая сборка через GitHub Actions

## Структура проекта

```
├── src/
│   ├── core/           # Core Engine (C)
│   │   ├── game_loop.c
│   │   ├── input_manager.c
│   │   └── game_state.c
│   ├── graphics/       # Graphics Engine (C++)
│   │   ├── renderer.cpp
│   │   ├── camera.cpp
│   │   └── model.cpp
│   ├── physics/        # Physics Engine (C++)
│   │   ├── physics_engine.cpp
│   │   ├── bunny_hop.cpp
│   │   └── collision_detector.cpp
│   ├── ui/            # UI Manager (C#)
│   │   ├── UIManager.cs
│   │   ├── Speedometer.cs
│   │   └── GameHUD.cs
│   ├── game_api.c     # Inter-language API
│   └── main.c         # Main entry point
├── include/
│   └── game_api.h     # Shared API definitions
└── CMakeLists.txt     # Build configuration
```

## Требования

### Для C/C++ компонентов:
- CMake 3.16+
- C11 compatible compiler
- C++17 compatible compiler
- OpenGL
- GLFW3

### Для C# компонента:
- .NET 6.0+

## Сборка

### Linux/macOS:
```bash
mkdir build
cd build
cmake ..
make
```

### Windows:
```cmd
mkdir build
cd build
cmake ..
cmake --build .
```

### C# компонент:
```bash
cd src/ui
dotnet build
```

## Запуск

```bash
./build/simple_shooter
```

## Автоматическая сборка

Проект настроен для автоматической сборки через GitHub Actions:

- ✅ **Мультиплатформенная сборка**: Windows, Linux, macOS
- ✅ **C/C++ компоненты**: CMake + OpenGL + GLFW
- ✅ **C# компонент**: .NET 6.0
- ✅ **Интеграционные тесты**: проверка совместимости компонентов
- ✅ **Релизные артефакты**: автоматическая упаковка для всех платформ

[Посмотреть статус сборки](https://github.com/Lox339/simple-c-shooter/actions)

## Разработка

Проект использует спецификацию в `.kiro/specs/simple-c-shooter/` для управления разработкой.

Следующие задачи для реализации:
1. ✅ Настройка проектной структуры
2. ⏳ Реализация Core Engine (C)
3. ⏳ Реализация Graphics & Physics Engine (C++)
4. ⏳ Реализация UI Manager (C#)

## Лицензия

MIT License