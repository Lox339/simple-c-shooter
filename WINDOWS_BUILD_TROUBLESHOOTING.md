# Windows Build Troubleshooting Guide

Это руководство поможет решить типичные проблемы при компиляции Simple C Shooter на Windows.

## Быстрая проверка

Запустите `quick-build.bat` для быстрой проверки сборки:
```cmd
quick-build.bat
```

## Требования

### Обязательные компоненты:
1. **Visual Studio 2022** или **Visual Studio Build Tools 2022**
2. **CMake 3.20+**
3. **Git**

### Опциональные (для полной функциональности):
4. **vcpkg** (для автоматической установки зависимостей)
5. **.NET 6.0 SDK** (для C# UI компонентов)

## Установка зависимостей

### Вариант 1: Использование vcpkg (рекомендуется)

```cmd
# Установка vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# Установка зависимостей
.\vcpkg install glfw3:x64-windows
.\vcpkg install glew:x64-windows
.\vcpkg install openal-soft:x64-windows
.\vcpkg install libsndfile:x64-windows
```

### Вариант 2: Ручная установка

Скачайте и установите библиотеки вручную:
- [GLFW](https://www.glfw.org/download.html)
- [GLEW](http://glew.sourceforge.net/)
- [OpenAL Soft](https://openal-soft.org/)

## Типичные ошибки и решения

### 1. "CMake не найден"
```
cmake : The term 'cmake' is not recognized...
```

**Решение:**
- Установите CMake с [официального сайта](https://cmake.org/download/)
- Добавьте CMake в PATH или перезапустите командную строку

### 2. "MSBuild не найден"
```
where msbuild
INFO: Could not find files for the given pattern(s).
```

**Решение:**
- Установите Visual Studio 2022 Community (бесплатно)
- Или установите Build Tools for Visual Studio 2022
- Убедитесь, что выбрана рабочая нагрузка "C++ build tools"

### 3. "GLFW не найден"
```
Could NOT find glfw3 (missing: glfw3_DIR)
```

**Решение:**
```cmd
# С vcpkg:
vcpkg install glfw3:x64-windows

# Или укажите путь вручную:
cmake -B build -DGLFW_ROOT="C:\path\to\glfw"
```

### 4. "GLEW не найден"
```
Could NOT find GLEW (missing: GLEW_LIBRARY GLEW_INCLUDE_DIR)
```

**Решение:**
```cmd
# С vcpkg:
vcpkg install glew:x64-windows

# Или укажите путь вручную:
cmake -B build -DGLEW_ROOT="C:\path\to\glew"
```

### 5. Ошибки линковки OpenGL
```
LINK : fatal error LNK1181: cannot open input file 'opengl32.lib'
```

**Решение:**
- Убедитесь, что установлен Windows SDK
- Переустановите Visual Studio с компонентом Windows SDK

### 6. "Не удается найти vcpkg.cmake"
```
include could not find load file: C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

**Решение:**
```cmd
# Укажите правильный путь к vcpkg:
cmake -B build -DCMAKE_TOOLCHAIN_FILE="C:\path\to\your\vcpkg\scripts\buildsystems\vcpkg.cmake"

# Или установите переменную окружения:
set VCPKG_ROOT=C:\path\to\your\vcpkg
```

### 7. Ошибки C# компиляции
```
error MSB3644: The reference assemblies for .NETFramework,Version=v6.0 were not found
```

**Решение:**
- Установите .NET 6.0 SDK с [официального сайта](https://dotnet.microsoft.com/download)
- Проверьте установку: `dotnet --version`

### 8. "Отказано в доступе" при сборке
```
LINK : fatal error LNK1104: cannot open file '...\simple_shooter.exe'
```

**Решение:**
- Закройте все экземпляры simple_shooter.exe
- Запустите командную строку от имени администратора
- Отключите антивирус временно

## Полная сборка

После решения проблем с зависимостями:

```cmd
# Конфигурация
cmake -B build -G "Visual Studio 17 2022" -A x64

# Сборка
cmake --build build --config Release --parallel

# Сборка C# компонентов (опционально)
dotnet restore src/ui/SimpleShooter.UI.csproj
dotnet build src/ui/SimpleShooter.UI.csproj --configuration Release
```

## Проверка сборки

```cmd
# Переход в директорию с исполняемым файлом
cd build\Release

# Запуск игры
simple_shooter.exe

# Или запуск с параметрами
simple_shooter.exe --help
```

## GitHub Actions

Для автоматической сборки на GitHub используется специальный workflow:
- `.github/workflows/windows-build.yml` - основная сборка для Windows
- `.github/workflows/build.yml` - кросс-платформенная сборка

## Получение помощи

Если проблемы не решаются:

1. Проверьте логи сборки в `build/` директории
2. Запустите `cmake --build build --config Release --verbose` для подробного вывода
3. Создайте issue в репозитории с полным текстом ошибки

## Полезные команды

```cmd
# Очистка сборки
rmdir /s /q build

# Проверка установленных компонентов
cmake --version
msbuild -version
dotnet --version
vcpkg version

# Список установленных пакетов vcpkg
vcpkg list

# Информация о системе
systeminfo | findstr /B /C:"OS Name" /C:"OS Version"
```