#include "game_api.h"
#include "game_loop.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Simple 3D Shooter - Starting...\n");
    printf("=================================\n");
    
    // Initialize game systems
    init_game();
    init_core_engine();
    
    printf("\nGame systems initialized successfully!\n");
    printf("Starting game loop test...\n\n");
    
    // Run the game loop
    run_game_loop();
    
    printf("\nGame loop test completed.\n");
    
    // Cleanup
    cleanup_core();
    cleanup_game();
    
    printf("Simple 3D Shooter - Exiting...\n");
    return 0;
}