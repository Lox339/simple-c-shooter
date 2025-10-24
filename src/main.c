#include "game_api.h"
#include "game_loop.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Simple 3D Shooter - Starting...\n");
    
    // Initialize game systems
    init_game();
    init_core_engine();
    
    printf("Game systems initialized successfully!\n");
    printf("Project structure is ready for development.\n");
    
    // Cleanup
    cleanup_core();
    cleanup_game();
    
    printf("Simple 3D Shooter - Exiting...\n");
    return 0;
}