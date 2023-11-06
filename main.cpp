#include <iostream>
#include <stdlib.h>
#include <stdbool.h>

#include "SDL.H"

typedef struct {
    SDL_Window *window;
} sdl_t;

bool init_sdl(sdl_t *sdl){
    if(SDL_InitSubSystem(SDL_INIT_TIMER || SDL_INIT_AUDIO || SDL_INIT_VIDEO) != 0){
        SDL_Log("Could not initialize SDL: s\n", SDL_GetError());
        return false; //SDL init fails
    }

    sdl -> window = SDL_CreateWindow("CHIP 8 Emulator", 640, 480, SDL_WINDOW_RESIZABLE);

    if(!SDL_CreateWindow()){
        SDL_Log("Could not initialize window: s\n", SDL_GetError());
        return false
    }

    return true; //SDL init success
}

void final_cleanup(void){
    SDL_DestroyWindow(window)
    SDL_Quit(); //Shut Down SDL
}

int main (int argc, char **argv){
    std::cout << "Testing" << std::endl;

    // Initialize SDL
    sdl_t sdl {0};
    if (!init_sdl(&sdl)){
        return 0;
    }

    // Create Window


    // Final cleanup
    final_cleanup();

}