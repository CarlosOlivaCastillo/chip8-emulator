#pragma once

#include <cstdint>
#include <array>
#include <string>

class Chip8 {
public:
    Chip8();
    void reset();
    bool loadROM(const std::string& filename);
    void cycle();
    void updateTimers();

    // Hardware specifications
    std::array<uint8_t, 4096> memory{};      // 4KB RAM
    std::array<uint8_t, 16> V{};             // V0 - VF General Purpose Registers
    uint16_t I{0};                           // Index Register
    uint16_t pc{0x200};                      // Program Counter

    std::array<uint16_t, 16> stack{};        // Call Stack
    uint8_t sp{0};                           // Stack Pointer

    uint8_t delayTimer{0};                   // 60 Hz Delay Timer
    uint8_t soundTimer{0};                   // 60 Hz Sound Timer

    std::array<uint8_t, 16> keypad{};        // 16-key Hexadecimal Keypad
    std::array<uint32_t, 64 * 32> display{}; // 64x32 Framebuffer
    
    // ESTA ES LA LÍNEA QUE FALTABA O ESTABA DANDO ERROR:
    bool drawFlag{false};
};
