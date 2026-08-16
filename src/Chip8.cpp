#include "Chip8.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <random>

const uint8_t FONTSET[80] = {
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

Chip8::Chip8() {
    reset();
}

void Chip8::reset() {
    memory.fill(0);
    V.fill(0);
    stack.fill(0);
    keypad.fill(0);
    display.fill(0);

    I = 0;
    pc = 0x200;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;
    drawFlag = true;

    std::memcpy(&memory[0x050], FONTSET, sizeof(FONTSET));
}

bool Chip8::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size <= 0 || size > static_cast<std::streamsize>(4096 - 0x200)) return false;

    file.read(reinterpret_cast<char*>(&memory[0x200]), size);
    return true;
}

void Chip8::updateTimers() {
    if (delayTimer > 0) --delayTimer;
    if (soundTimer > 0) --soundTimer;
}

void Chip8::cycle() {
    if (pc >= 4094) return;

    // Fetch Opcode (16-bit Big-Endian)
    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];

    // Decode Opcode
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t  nn  = opcode & 0x00FF;
    uint8_t  n   = opcode & 0x000F;
    uint8_t  x   = (opcode >> 8) & 0x000F;
    uint8_t  y   = (opcode >> 4) & 0x000F;
    uint8_t  prefix = (opcode >> 12) & 0x000F;

    pc += 2;

    // Execute Instruction
    switch (prefix) {
        case 0x0:
            if (opcode == 0x00E0) {
                display.fill(0);
                drawFlag = true;
            } else if (opcode == 0x00EE) {
                if (sp > 0) {
                    --sp;
                    pc = stack[sp];
                }
            }
            break;

        case 0x1:
            pc = nnn;
            break;

        case 0x2:
            if (sp < stack.size()) {
                stack[sp] = pc;
                ++sp;
                pc = nnn;
            }
            break;

        case 0x3:
            if (V[x] == nn) pc += 2;
            break;

        case 0x4:
            if (V[x] != nn) pc += 2;
            break;

        case 0x5:
            if (V[x] == V[y]) pc += 2;
            break;

        case 0x6:
            V[x] = nn;
            break;

        case 0x7:
            V[x] += nn;
            break;

        case 0x8:
            switch (n) {
                case 0x0: V[x] = V[y]; break;
                case 0x1: V[x] |= V[y]; break;
                case 0x2: V[x] &= V[y]; break;
                case 0x3: V[x] ^= V[y]; break;
                case 0x4: {
                    uint16_t sum = V[x] + V[y];
                    V[0xF] = (sum > 255) ? 1 : 0;
                    V[x] = sum & 0xFF;
                    break;
                }
                case 0x5: {
                    uint8_t notBorrow = (V[x] >= V[y]) ? 1 : 0;
                    V[x] -= V[y];
                    V[0xF] = notBorrow;
                    break;
                }
                case 0x6: {
                    uint8_t lsb = V[x] & 0x1;
                    V[x] >>= 1;
                    V[0xF] = lsb;
                    break;
                }
                case 0x7: {
                    uint8_t notBorrow = (V[y] >= V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                    V[0xF] = notBorrow;
                    break;
                }
                case 0xE: {
                    uint8_t msb = (V[x] >> 7) & 0x1;
                    V[x] <<= 1;
                    V[0xF] = msb;
                    break;
                }
                default: break;
            }
            break;

        case 0x9:
            if (V[x] != V[y]) pc += 2;
            break;

        case 0xA:
            I = nnn;
            break;

        case 0xB:
            pc = nnn + V[0];
            break;

        case 0xC: {
            static std::mt19937 gen(1337);
            static std::uniform_int_distribution<uint8_t> dist(0, 255);
            V[x] = dist(gen) & nn;
            break;
        }

        case 0xD: {
            uint8_t posX = V[x] % 64;
            uint8_t posY = V[y] % 32;
            V[0xF] = 0;

            for (size_t row = 0; row < n; ++row) {
                if (I + row >= memory.size()) break;
                uint8_t spriteByte = memory[I + row];
                for (size_t col = 0; col < 8; ++col) {
                    if ((spriteByte & (0x80 >> col)) != 0) {
                        size_t screenX = (posX + col) % 64;
                        size_t screenY = (posY + row) % 32;
                        size_t index = screenY * 64 + screenX;

                        if (display[index] == 0xFFFFFFFF) {
                            V[0xF] = 1;
                        }
                        display[index] ^= 0xFFFFFFFF;
                    }
                }
            }
            drawFlag = true;
            break;
        }

        case 0xE: {
            uint8_t key = V[x] & 0x0F;
            if (nn == 0x9E) {
                if (keypad[key] != 0) pc += 2;
            } else if (nn == 0xA1) {
                if (keypad[key] == 0) pc += 2;
            }
            break;
        }

        case 0xF:
            switch (nn) {
                case 0x07: V[x] = delayTimer; break;
                case 0x15: delayTimer = V[x]; break;
                case 0x18: soundTimer = V[x]; break;
                case 0x1E: I += V[x]; break;
                case 0x29: I = 0x050 + ((V[x] & 0x0F) * 5); break;
                case 0x33:
                    if (I + 2 < memory.size()) {
                        memory[I]     = V[x] / 100;
                        memory[I + 1] = (V[x] / 10) % 10;
                        memory[I + 2] = V[x] % 10;
                    }
                    break;
                case 0x55:
                    for (int i = 0; i <= x && (I + i) < 4096; ++i) memory[I + i] = V[i];
                    break;
                case 0x65:
                    for (int i = 0; i <= x && (I + i) < 4096; ++i) V[i] = memory[I + i];
                    break;
                case 0x0A: {
                    bool pressed = false;
                    for (int i = 0; i < 16; ++i) {
                        if (keypad[i] != 0) {
                            V[x] = i;
                            pressed = true;
                            break;
                        }
                    }
                    if (!pressed) pc -= 2;
                    break;
                }
                default: break;
            }
            break;

        default: break;
    }
}
