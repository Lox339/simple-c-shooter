// Simple Shooter - Main Entry Point
// Multi-language 3D shooter with bunny hop mechanics

#include "core/game_loop.h"
#include "core/game_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Version information
#define GAME_VERSION_MAJOR 1
#define GAME_VERSION_MINOR 0
#define GAME_VERSION_PATCH 0
#define GAME_VERSION_STRING "1.0.0"

// Command line options
static void print_usage(const char* program_name) {
    printf("Simple Shooter v%s\n", GAME_VERSION_STRING);
    printf("3D Shooter with Bunny Hop Mechanics\n\n");
    printf("Usage: %s [options]\n\n", program_name);
    printf("Options:\n");
    printf("  --help, -h        Show this help message\n");
    printf("  --version, -v     Show version information\n");
    printf("  --test            Run in test mode (exits after initialization)\n");
    printf("  --fps <number>    Set target FPS (default: 60)\n");
    printf("  --windowed        Force windowed mode\n");
    printf("  --fullscreen      Force fullscreen mode\n");
    printf("  --no-audio        Disable audio system\n");
    printf("  --debug           Enable debug output\n");
    printf("\nControls:\n");
    printf("  WASD              Move player\n");
    printf("  Mouse             Look around\n");
    printf("  Space             Jump (chain for bunny hopping)\n");
    printf("  Left Click        Shoot\n");
    printf("  ESC               Pause/Resume game\n");
    printf("  Q                 Quit game\n");
    printf("  O                 Open audio settings\n");
    printf("\nFeatures:\n");
    printf("  - Advanced bunny hop mechanics\n");
    printf("  - Real-time speedometer\n");
    printf("  - 3D positional audio\n");
    printf("  - Smart AI enemies\n");
    printf("  - Multi-language architecture (C/C++/C#)\n");
    printf("\n");
}

static void print_version() {
    printf("Simple Shooter v%s\n", GAME_VERSION_STRING);
    printf("Build: %s %s\n", __DATE__, __TIME__);
    printf("Architecture: C/C++/C# Multi-language\n");
    printf("Graphics: OpenGL 3.3+\n");
    printf("Audio: OpenAL/Custom Audio System\n");
    printf("Physics: Custom Physics Engine with Bunny Hop\n");
    printf("UI: C# with OpenGL Integration\n");
}

// Global configuration
typedef struct {
    int test_mode;
    int target_fps;
    int windowed_mode;
    int fullscreen_mode;
    int no_audio;
    int debug_mode;
} GameConfig;

static GameConfig g_config = {
    .test_mode = 0,
    .target_fps = 60,
    .windowed_mode = 0,
    .fullscreen_mode = 0,
    .no_audio = 0,
    .debug_mode = 0
};

// Parse command line arguments
static int parse_arguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            print_version();
            return 0;
        }
        else if (strcmp(argv[i], "--test") == 0) {
            g_config.test_mode = 1;
        }
        else if (strcmp(argv[i], "--fps") == 0) {
            if (i + 1 < argc) {
                g_config.target_fps = atoi(argv[++i]);
                if (g_config.target_fps <= 0 || g_config.target_fps > 300) {
                    printf("Error: Invalid FPS value. Must be between 1 and 300.\n");
                    return -1;
                }
            } else {
                printf("Error: --fps requires a number argument.\n");
                return -1;
            }
        }
        else if (strcmp(argv[i], "--windowed") == 0) {
            g_config.windowed_mode = 1;
        }
        else if (strcmp(argv[i], "--fullscreen") == 0) {
            g_config.fullscreen_mode = 1;
        }
        else if (strcmp(argv[i], "--no-audio") == 0) {
            g_config.no_audio = 1;
        }
        else if (strcmp(argv[i], "--debug") == 0) {
            g_config.debug_mode = 1;
        }
        else {
            printf("Error: Unknown option '%s'\n", argv[i]);
            printf("Use --help for usage information.\n");
            return -1;
        }
    }
    
    return 1; // Continue execution
}

// Initialize game systems
static int initialize_game() {
    printf("=== Simple Shooter v%s ===\n", GAME_VERSION_STRING);
    printf("Initializing game systems...\n");
    
    // Apply configuration
    if (g_config.target_fps != 60) {
        set_target_fps(g_config.target_fps);
        printf("Target FPS set to: %d\n", g_config.target_fps);
    }
    
    if (g_config.debug_mode) {
        printf("Debug mode enabled\n");
    }
    
    if (g_config.no_audio) {
        printf("Audio system disabled by command line option\n");
    }
    
    // Initialize core engine
    init_core_engine();
    
    printf("Game systems initialized successfully!\n");
    printf("=================================\n");
    
    return 1;
}

// Main game entry point
int main(int argc, char* argv[]) {
    printf("Starting Simple Shooter...\n");
    
    // Parse command line arguments
    int parse_result = parse_arguments(argc, argv);
    if (parse_result <= 0) {
        return parse_result < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    }
    
    // Initialize game
    if (!initialize_game()) {
        printf("Failed to initialize game systems!\n");
        return EXIT_FAILURE;
    }
    
    // Test mode - just initialize and exit
    if (g_config.test_mode) {
        printf("Test mode: Initialization successful, exiting.\n");
        cleanup_core();
        return EXIT_SUCCESS;
    }
    
    // Print startup information
    printf("\nGame ready! Starting main loop...\n");
    printf("Use --help for controls and options.\n");
    printf("Have fun bunny hopping!\n\n");
    
    // Run main game loop
    int exit_code = EXIT_SUCCESS;
    
    try {
        run_game_loop();
        printf("\nGame loop ended normally.\n");
    }
    catch (...) {
        printf("\nGame loop ended with error.\n");
        exit_code = EXIT_FAILURE;
    }
    
    // Cleanup
    printf("Cleaning up game systems...\n");
    cleanup_core();
    
    printf("Simple Shooter shutdown complete.\n");
    printf("Thanks for playing!\n");
    
    return exit_code;
}

// Error handling for C++ exceptions in C code
#ifdef __cplusplus
extern "C" {
#endif

void handle_fatal_error(const char* message) {
    printf("FATAL ERROR: %s\n", message);
    printf("The game will now exit.\n");
    
    // Try to cleanup gracefully
    cleanup_core();
    
    exit(EXIT_FAILURE);
}

#ifdef __cplusplus
}
#endif