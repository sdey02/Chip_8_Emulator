#include <iostream>
#include <stdlib.h>
#include <stdbool.h>

#include "SDL.H"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

bool init_sdl(sdl_t *sdl){
    if(SDL_InitSubSystem(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0){ //Checks weather SDL Init works
        SDL_Log("Could not initialize SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    sdl ->window = SDL_CreateWindow("CHIP 8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_RESIZABLE); //Initializes the window

    if(!sdl ->window){
        SDL_Log("Could not initialize window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    sdl ->renderer = SDL_CreateRenderer(sdl ->window, -1, SDL_RENDERER_ACCELERATED); //Initializes the render

    if(!sdl ->renderer){
        SDL_Log("Could not initialize renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;; //SDL init success
}

void final_cleanup(sdl_t *sdl){
    SDL_DestroyWindow(sdl -> window);
    SDL_Quit(); //Shut Down SDL
}

int main (int argc, char **argv){
    // Initialize SDL
    sdl_t sdl = {0};
    if (!init_sdl(&sdl)){
        exit(EXIT_FAILURE);
    }

    // Create Window


    // Final cleanup
    final_cleanup(&sdl);
}