#include "Chip8.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <thread>

void handleKey(Chip8& chip8, SDL_Keycode key, uint8_t state) {
    switch (key) {
        // Hexadecimal keypad mapping
        case SDLK_1: chip8.keypad[0x1] = state; break;
        case SDLK_2: chip8.keypad[0x2] = state; break;
        case SDLK_3: chip8.keypad[0x3] = state; break;
        case SDLK_4: chip8.keypad[0xC] = state; break;

        case SDLK_q: chip8.keypad[0x4] = state; break;
        case SDLK_w: chip8.keypad[0x5] = state; break;
        case SDLK_e: chip8.keypad[0x6] = state; break;
        case SDLK_r: chip8.keypad[0xD] = state; break;

        case SDLK_a: chip8.keypad[0x7] = state; break;
        case SDLK_s: chip8.keypad[0x8] = state; break;
        case SDLK_d: chip8.keypad[0x9] = state; break;
        case SDLK_f: chip8.keypad[0xE] = state; break;

        case SDLK_z: chip8.keypad[0xA] = state; break;
        case SDLK_x: chip8.keypad[0x0] = state; break;
        case SDLK_c: chip8.keypad[0xB] = state; break;
        case SDLK_v: chip8.keypad[0xF] = state; break;

        // Directional fallbacks
        case SDLK_LEFT:  chip8.keypad[0x4] = state; chip8.keypad[0x7] = state; break;
        case SDLK_RIGHT: chip8.keypad[0x6] = state; chip8.keypad[0x9] = state; break;
        case SDLK_SPACE: chip8.keypad[0x5] = state; chip8.keypad[0x8] = state; break;
        case SDLK_UP:    chip8.keypad[0x1] = state; break;
        case SDLK_DOWN:  chip8.keypad[0x4] = state; break;
        default: break;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./chip8_emulator <path_to_rom.ch8>\n";
        return 1;
    }

    Chip8 chip8;
    if (!chip8.loadROM(argv[1])) {
        std::cerr << "Error: Unable to load ROM " << argv[1] << "\n";
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    const int SCALE = 15;
    std::string title = std::string("CHIP-8 Emulator - ") + argv[1];
    SDL_Window* window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        64 * SCALE, 32 * SCALE,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        64, 32
    );

    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                handleKey(chip8, event.key.keysym.sym, 1);
            } else if (event.type == SDL_KEYUP) {
                handleKey(chip8, event.key.keysym.sym, 0);
            }
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastFrameTime).count();

        if (dt >= 16.66f) {
            lastFrameTime = currentTime;

            for (int i = 0; i < 10; ++i) {
                chip8.cycle();
            }

            chip8.updateTimers();

            if (chip8.drawFlag) {
                chip8.drawFlag = false;
                SDL_UpdateTexture(texture, nullptr, chip8.display.data(), 64 * sizeof(uint32_t));
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, nullptr, nullptr);
                SDL_RenderPresent(renderer);
            }
        }

        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
