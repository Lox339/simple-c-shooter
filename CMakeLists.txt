cmake_minimum_required(VERSION 3.16)
project(SimpleShooter VERSION 1.0.0 LANGUAGES C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Include directories
include_directories(include)
include_directories(src/core)
include_directories(src/graphics)
include_directories(src/physics)

# Find required packages
find_package(OpenGL REQUIRED)

# Try to find GLFW (optional)
find_package(glfw3 QUIET)
if(glfw3_FOUND)
    message(STATUS "GLFW found - enabling window support")
    add_definitions(-DGLFW_AVAILABLE)
else()
    message(STATUS "GLFW not found - using software rendering fallback")
endif()

# Core Engine (C) sources
set(CORE_SOURCES
    src/core/game_loop.c
    src/core/input_manager.c
    src/core/game_state.c
    src/core/object_manager.c
    src/game_api.c
)

# Graphics & Physics Engine (C++) sources
set(GRAPHICS_SOURCES
    src/graphics/renderer.cpp
    src/graphics/camera.cpp
    src/graphics/model.cpp
    src/graphics/math_utils.cpp
    src/graphics_bridge.cpp
    src/physics/physics_engine.cpp
    src/physics/bunny_hop.cpp
    src/physics/collision_detector.cpp
)

# Create static libraries
add_library(core_engine STATIC ${CORE_SOURCES})
add_library(graphics_engine STATIC ${GRAPHICS_SOURCES})

# Link OpenGL to graphics engine
target_link_libraries(graphics_engine OpenGL::GL)

# Link GLFW if available
if(glfw3_FOUND)
    target_link_libraries(graphics_engine glfw)
endif()

# Main executable
add_executable(simple_shooter src/main.c)

# Link libraries to main executable
target_link_libraries(simple_shooter core_engine graphics_engine)

# Compiler-specific options
if(MSVC)
    target_compile_options(simple_shooter PRIVATE /W4)
else()
    target_compile_options(simple_shooter PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug symbols
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(simple_shooter PRIVATE DEBUG=1)
endif()

# Install targets
install(TARGETS simple_shooter DESTINATION bin)
install(FILES include/game_api.h DESTINATION include)