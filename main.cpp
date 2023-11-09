#include <iostream>
#include <stdlib.h>
#include <stdbool.h>

#include "SDL.H"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event *event;
} sdl_t;

// Emulator configuration object
typedef struct {
    uint32_t window_width;      // SDL window width
    uint32_t window_height;     // SDL window height
    uint32_t fg_color;          // Foreground color RGBA8888
    uint32_t bg_color;          // Background color RGBA8888
    uint32_t scale_factor;      // Amount to scale Chip8 pixel by
} config_t;

typedef enum {
    Quit,
    Running,
    Pause,
} emulator_state_t;

typedef struct {
 emulator_state_t state;
} chip_8_t;

bool init_sdl(sdl_t *sdl, config_t config){
    if(SDL_InitSubSystem(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0){ //Checks weather SDL Init works
        SDL_Log("Could not initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    sdl ->window = SDL_CreateWindow("CHIP 8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config.window_width * config.scale_factor, config.window_height * config.scale_factor, 0); //Initializes the window

    if(!sdl ->window){
        SDL_Log("Could not initialize window: %s\n", SDL_GetError());
        return false;
    }

    sdl ->renderer = SDL_CreateRenderer(sdl ->window, -1, SDL_RENDERER_ACCELERATED); //Initializes the render

    if(!sdl ->renderer){
        SDL_Log("Could not initialize renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return true; //SDL init success
}

// Set up initial emulator configuration from passed in arguments
bool set_config_from_args(config_t *config, const int argc, char **argv) {

    // Set defaults
    *config = (config_t){
        .window_width  = 64,    // CHIP8 original X resolution
        .window_height = 32,    // CHIP8 original Y resolution
        .fg_color = 0xFFFFFFFF, // WHite
        .bg_color = 0xFFFF00FF, // Yellow
        .scale_factor = 20
    };

    // Override defaults from passed in arguments
    for (int i = 1; i < argc; i++) {
            (void)argv[i];  // Prevent compiler error from unused variables argc/argv
    }

    return true;    // Success
}

bool init_chip8 (chip_8_t *chip8){
    return true;
}

void final_cleanup(sdl_t *sdl){
    SDL_DestroyRenderer(sdl -> renderer);
    SDL_DestroyWindow(sdl -> window);
    SDL_Quit(); //Shut Down SDL
}

void clear_screen(const sdl_t sdl, const config_t config){
    // RGBA is 32 bits
    const uint8_t r = (config.bg_color >> 24) & 0xFF; //Set the of R
    const uint8_t g = (config.bg_color >> 16) & 0xFF; //Set the of G
    const uint8_t b = (config.bg_color >> 8) & 0xFF; //Set the of B
    const uint8_t a = (config.bg_color >> 0) & 0xFF; //Set the of A

    SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
    SDL_RenderClear(sdl.renderer);
}

void update_screen(const sdl_t sdl){
    SDL_RenderPresent(sdl.renderer);
}

void handle_input(chip_8_t *chip8){
    SDL_Event event;

    while (SDL_PollEvent(&event)){
        switch (event.type)
        {
        case (SDL_KEYDOWN):
            switch (event.key.keysym.sym) {
                case (SDLK_ESCAPE):
                    chip8 ->state = Quit;
                    return;
                default:
                    break;
            }
            break;

        case (SDL_KEYUP):
            /* code */
            break;

        case (SDL_QUIT):
            chip8 ->state = Quit;
            break;
        }
    } 
}

int main (int argc, char **argv){
    // Initialize Config
    config_t config = {0};
    if (!set_config_from_args(&config, argc, argv)){
        exit(EXIT_FAILURE);
    }

    // Initialize SDL
    sdl_t sdl = {0};
    if (!init_sdl(&sdl, config)){
        exit(EXIT_FAILURE);
    }

    // Initialize Chip-8 Machine
    chip_8_t chip8;
    if (!init_chip8(&chip8)){
        exit(EXIT_FAILURE);
    }
    // Intitial screen clear
    clear_screen(sdl, config);

    // Main Loop
    while (chip8.state != Quit){
        // Handle User Input
        handle_input(&chip8);

        // Delay for 60FPS
        SDL_Delay(16);

        // Update Screen
        update_screen(sdl);
        }

    // Final cleanup
    final_cleanup(&sdl);
}