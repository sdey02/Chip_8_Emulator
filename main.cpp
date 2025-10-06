#include <iostream>
#include <stdlib.h>
#include <stdbool.h>

#include "SDL.H"

typedef struct {
    SDL_Window * window;
    SDL_Renderer *renderer;
    SDL_Event *event;
} sdl_t;

// Emulator configuration object
typedef struct {
    uint32_t window_width;      // * SDL window width
    uint32_t window_height;     // * SDL window height
    uint32_t fg_color;          // * Foreground color RGBA8888
    uint32_t bg_color;          // * Background color RGBA8888
    uint32_t scale_factor;      // * Amount to scale Chip8 pixel by
} config_t;

// Emulator states
typedef enum {
    Quit,
    Running,
    Pause,
} emulator_state_t;

// CHIP8 Instruction Format
typedef struct {
    uint16_t opcodes;
    uint16_t NNN;       // 12 Bit Constant
    uint8_t NN;         // 8 Bit Constant
    uint8_t N;          // 4 Bit Constant
    uint8_t X;          // 4 Bit Register Identifier
    uint8_t Y;          // 4 Bit Register Identifier
} instruction_t;

// Chip_8 Emulator Framework
typedef struct {
    emulator_state_t state;
    uint8_t ram[4096];
    bool dislay;             // Uppermost 256 bytes 0xF00-0xFFF
    uint16_t call_stack[12];
    uint8_t V[16];           // Data Registers V0-Vf
    uint16_t I;              // Index Register
    uint16_t PC;             // Index Register
    uint8_t delay_timer;     // Decrement at 60hz when >0
    uint8_t sound_timer;     // Decrement at 60hz when >0 + play tone
    bool keypad[16];         // Keypad
    const char *rom_name;    // Rom Name
    instruction_t inst;      // Current Instruction
} chip_8_t;

void chip8_opcode_00E0(Chip8 *chip8) {                              // 0x00E0: Clear the screen
    memset(&chip8->display[0], false, sizeof chip8->display);
    chip8->draw = true;                                             // Will update screen on next 60hz tick

}

void chip8_opcode_00EE(Chip8 *chip8) {
    // ... Implement opcode 0x00EE functionality ...
}

// ... Add more opcode functions ...

Chip8OpcodeFunction chip8_opcode_functions[] = {
    chip8_opcode_00E0,
    chip8_opcode_00EE,
    // ... Add more opcode functions ...
};



// Emulator Initialization Function
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

bool init_chip8 (chip_8_t *chip8, const char rom_name[]){
    const uint32_t entry_point = 0x200 // Loaded to x200
    const uint8_t font[] {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // Load Font
    memcpy (&chip8 ->ram[0], font, sizeof(font));

    // Load ROM to CHIP 8
    // Open ROM Size
    FILE *rom = fopen(rom_name, "rb");
    if (!rom){
        SDL_LOG("ROM FILE INVALID")
        return false
    }

    // Get and Check ROM SIZE
    fseek(rom, 0, SEEK_END)
    const size_t rom_size = tell(rom);
    const size_t max_size = sizeof chip8 ->ram - entry_point;
    rewind(rom)

    if (rom_size > max_size){
        SDL_LOG("ROM TOO BIG")
        return false
    }

    if(fread(&chip8 ->ram[entry_point], rom_size, 1, rom) != 1){
        SDL_LOG("Could not read ROM into CHIP8 memory.")
        return false
    }

    fclose(rom)

    //CIP8 State
    chip8 ->state = Running;
    chip8 ->PC = entry_point;
    chip8 ->rom_name = rom_name;

    return true;
}

// Ends all SDL processes
void final_cleanup(sdl_t *sdl){
    SDL_DestroyRenderer(sdl -> renderer); // Destroys the renderer
    SDL_DestroyWindow(sdl -> window);     // Destroys the window
    SDL_Quit();                           // Shut Down SDL 
}

void clear_screen(const sdl_t sdl, const config_t config){
    // RGBA is 32 bits
    const uint8_t r = (config.bg_color >> 24) & 0xFF; //Set the of R
    const uint8_t g = (config.bg_color >> 16) & 0xFF; //Set the of G
    const uint8_t b = (config.bg_color >> 8) & 0xFF;  //Set the of B
    const uint8_t a = (config.bg_color >> 0) & 0xFF;  //Set the of A

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
                case (SDLK_SPACE):
                    if(chip8 ->Running){
                        chip8 ->state = Pause;                   // Paused
                        puts ("========= Paused =========")
                    } else{
                        chip8 ->state = Running;                 // Resume        
                    }
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

// 1 CHIP* Instruction Set Emulation
void emulate_instruction(chip8_t *chip8){
    // Get next opcode from ram
    chip8 ->inst.opcode = chip8 ->ram[chip8 ->PC] << 8 | chip8 ->ram[chip8 ->PC + 1]
    PC += 2; // Preincrement OPCODES

    // Fill out current instruction format
    chip8->inst.NNN = chip8->inst.opcode & 0x0FFF;
    chip8->inst.NN = chip8->inst.opcode & 0x0FF;
    chip8->inst.N = chip8->inst.opcode & 0x0F;
    chip8->inst.X = (chip8->inst.opcode >> 8) & 0x0F;
    chip8->inst.Y = (chip8->inst.opcode >> 4) & 0x0F;
}

int main (int argc, char **argv){
    // Default Usage Message
    if(argc < 2){
        fprintf(stderr, "Error");
        exit(EXIT_FAILURE);
    }

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
    const char *rom_name[] = argv[1];
    if (!init_chip8(&chip8, rom_name)){
        exit(EXIT_FAILURE);
    }
    // Intitial screen clear
    clear_screen(sdl, config);

    // Main Loop
    while (chip8.state != Quit){
        // Handle User Input
        handle_input(&chip8);

        if (chip8.state == Paused){
            continue;
        }

        // Emulate CHIP8 Instruction
        emulate_instruction(&chip8);

        // Delay for 60FPS
        SDL_Delay(16);

        // Update Screen
        update_screen(sdl);
        }

    // Final cleanup
    final_cleanup(&sdl);
}