cmake_minimum_required(VERSION 3.20)
project(SimpleShooter VERSION 1.0.0 LANGUAGES C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3 -DNDEBUG)
    endif()
endif()

# Platform-specific definitions
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
    set(PLATFORM_LIBS opengl32 glu32)
elseif(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    set(PLATFORM_LIBS ${OPENGL_LIBRARY} ${COCOA_LIBRARY})
else()
    set(PLATFORM_LIBS GL GLU X11 pthread dl)
endif()

# Find required packages
find_package(PkgConfig)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(glfw3 REQUIRED)
if(NOT glfw3_FOUND)
    message(STATUS "GLFW not found via find_package, trying pkg-config")
    pkg_check_modules(GLFW REQUIRED glfw3)
endif()

# Find GLEW
find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
    message(STATUS "GLEW not found via find_package, trying pkg-config")
    pkg_check_modules(GLEW REQUIRED glew)
endif()

# Find OpenAL (optional for audio)
find_package(OpenAL)
if(NOT OpenAL_FOUND)
    message(STATUS "OpenAL not found, audio will be disabled")
    add_definitions(-DNO_AUDIO)
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${OPENGL_INCLUDE_DIRS}
)

if(GLFW_FOUND)
    include_directories(${GLFW_INCLUDE_DIRS})
endif()

if(GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
endif()

if(OpenAL_FOUND)
    include_directories(${OPENAL_INCLUDE_DIR})
endif()

# Core Engine (C) sources
set(CORE_SOURCES
    src/core/game_loop.c
    src/core/game_state.c
    src/core/input_manager.c
    src/core/object_manager.c
    src/core/enemy_ai.c
    src/core/collision_system.c
)

# Graphics Engine (C++) sources
set(GRAPHICS_SOURCES
    src/graphics/renderer.cpp
    src/graphics/camera.cpp
    src/graphics/model.cpp
    src/graphics/math_utils.cpp
    src/graphics/ui_renderer.cpp
    src/graphics/projectile_trail.cpp
    src/graphics/hit_effects.cpp
    src/graphics_bridge.cpp
)

# Physics Engine (C++) sources
set(PHYSICS_SOURCES
    src/physics/physics_engine.cpp
    src/physics/collision_detector.cpp
    src/physics/bunny_hop.cpp
    src/physics_bridge.cpp
)

# Audio System (C) sources
set(AUDIO_SOURCES
    src/audio/audio_system.c
    src/audio/sound_generator.c
    src/audio_bridge.c
)

# UI Bridge (C++) sources
set(UI_BRIDGE_SOURCES
    src/ui_bridge.cpp
)

# Main executable sources
set(MAIN_SOURCES
    src/main.c
)

# Create the main executable
add_executable(simple_shooter
    ${MAIN_SOURCES}
    ${CORE_SOURCES}
    ${GRAPHICS_SOURCES}
    ${PHYSICS_SOURCES}
    ${AUDIO_SOURCES}
    ${UI_BRIDGE_SOURCES}
)

# Link libraries
target_link_libraries(simple_shooter
    ${OPENGL_LIBRARIES}
    ${PLATFORM_LIBS}
)

# Link GLFW
if(TARGET glfw)
    target_link_libraries(simple_shooter glfw)
elseif(GLFW_FOUND)
    target_link_libraries(simple_shooter ${GLFW_LIBRARIES})
endif()

# Link GLEW
if(TARGET GLEW::GLEW)
    target_link_libraries(simple_shooter GLEW::GLEW)
elseif(GLEW_FOUND)
    target_link_libraries(simple_shooter ${GLEW_LIBRARIES})
endif()

# Link OpenAL if available
if(OpenAL_FOUND)
    target_link_libraries(simple_shooter ${OPENAL_LIBRARY})
endif()

# Platform-specific linking
if(WIN32)
    target_link_libraries(simple_shooter winmm)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(simple_shooter m)
endif()

# Create shared library for C# interop
add_library(game_core SHARED
    ${CORE_SOURCES}
    ${GRAPHICS_SOURCES}
    ${PHYSICS_SOURCES}
    ${AUDIO_SOURCES}
    ${UI_BRIDGE_SOURCES}
)

target_link_libraries(game_core
    ${OPENGL_LIBRARIES}
    ${PLATFORM_LIBS}
)

if(TARGET glfw)
    target_link_libraries(game_core glfw)
elseif(GLFW_FOUND)
    target_link_libraries(game_core ${GLFW_LIBRARIES})
endif()

if(TARGET GLEW::GLEW)
    target_link_libraries(game_core GLEW::GLEW)
elseif(GLEW_FOUND)
    target_link_libraries(game_core ${GLEW_LIBRARIES})
endif()

if(OpenAL_FOUND)
    target_link_libraries(game_core ${OPENAL_LIBRARY})
endif()

if(WIN32)
    target_link_libraries(game_core winmm)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(game_core m)
endif()

# Set output directories
set_target_properties(simple_shooter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(game_core PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Install targets
install(TARGETS simple_shooter game_core
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install assets if they exist
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
        DESTINATION .
    )
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/sounds)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/sounds
        DESTINATION .
    )
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/music)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/music
        DESTINATION .
    )
endif()

# Install documentation
install(FILES README.md
    DESTINATION .
)

# Enable testing
enable_testing()

# Add basic smoke test
add_test(NAME smoke_test
    COMMAND simple_shooter --test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Set test timeout
set_tests_properties(smoke_test PROPERTIES TIMEOUT 30)

# CPack configuration for packaging
include(CPack)
set(CPACK_PACKAGE_NAME "SimpleShooter")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "3D Shooter with Bunny Hop Mechanics")
set(CPACK_PACKAGE_VENDOR "SimpleShooter Team")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "Simple Shooter Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  OpenGL: ${OPENGL_FOUND}")
message(STATUS "  GLFW: ${glfw3_FOUND}")
message(STATUS "  GLEW: ${GLEW_FOUND}")
message(STATUS "  OpenAL: ${OpenAL_FOUND}")
message(STATUS "")