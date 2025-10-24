#include "input_manager.h"
#include <stdio.h>
#include <string.h>

static InputState g_input_state;

void init_input_manager() {
    memset(&g_input_state, 0, sizeof(InputState));
    g_input_state.mouse_sensitivity = 1.0f;
    printf("Input Manager initialized\n");
}

void process_input() {
    // Placeholder for input processing
    // This will be implemented in task 2.2
}

InputState* get_input_state() {
    return &g_input_state;
}

void cleanup_input_manager() {
    printf("Input Manager cleaned up\n");
}