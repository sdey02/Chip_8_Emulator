#include <iostream>
#include <stdlib.h>
#include <stdbool.h>

#include "SDL.H"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

// Emulator configuration object
typedef struct {
    uint32_t window_width;      // SDL window width
    uint32_t window_height;     // SDL window height
    uint32_t fg_color;          // Foreground color RGBA8888
    uint32_t bg_color;          // Background color RGBA8888
} config_t;

bool init_sdl(sdl_t *sdl, config_t config){
    if(SDL_InitSubSystem(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0){ //Checks weather SDL Init works
        SDL_Log("Could not initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    sdl ->window = SDL_CreateWindow("CHIP 8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config.window_width, config.window_height, SDL_WINDOW_RESIZABLE); //Initializes the window

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
        .fg_color = 0xFFFF00FF, // Yellow
        .bg_color = 0x00000000, // BLACK
    };

    // Override defaults from passed in arguments
    for (int i = 1; i < argc; i++) {
            (void)argv[i];  // Prevent compiler error from unused variables argc/argv
    }

    return true;    // Success
}

void final_cleanup(sdl_t *sdl){
    SDL_DestroyRenderer(sdl -> renderer);
    SDL_DestroyWindow(sdl -> window);
    SDL_Quit(); //Shut Down SDL
}

void clear_screen(const config_t config){
    const uint8_t r = (config.bg_color << 24);
    // int SDL_SetRenderDrawColor(sdl.renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    int SDL_RenderClear(sdl.renderer);
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

    // Intitial screen clear
    clear_screen(config);

    // Main Loop
    while (true){

    }

    // Final cleanup
    final_cleanup(&sdl);
}