<div align="center">

# CHIP-8 Virtual Machine & Interpreter

![C++17](https://img.shields.io/badge/C%2B%2B17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![SDL2](https://img.shields.io/badge/SDL2-228B22?style=for-the-badge&logo=libsdl&logoColor=white)
![Build Status](https://img.shields.io/github/actions/workflow/status/CarlosOlivaCastillo/chip8-emulator/cmake.yml?branch=main&style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)

> A highly accurate, low-level CHIP-8 emulator written in modern C++17 using SDL2.

![Space Invaders](https://upload.wikimedia.org/wikipedia/commons/d/d5/Space_invaders.gif)

</div>

---

## 🚀 Quick Start & Build Instructions

This project uses **CMake** for cross-platform builds and depends on **SDL2** for hardware-accelerated rendering and input polling.

### Linux / WSL (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install libsdl2-dev
mkdir -p build && cd build
cmake ..
make
./chip8_emulator ../roms/space_invaders.ch8
```
🎮 Controls & Keypad LayoutThe original CHIP-8 used a 16-key hexadecimal keypad. This emulator maps it to the modern QWERTY layout, with added directional fallbacks for modern convenience.Default MappingPlaintextCHIP-8 Keypad          Modern Keyboard
+-+-+-+-+              +-+-+-+-+
|1|2|3|C|              |1|2|3|4|
+-+-+-+-+              +-+-+-+-+
|4|5|6|D|    --->      |Q|W|E|R|
+-+-+-+-+              +-+-+-+-+
|7|8|9|E|              |A|S|D|F|
+-+-+-+-+              +-+-+-+-+
|A|0|B|F|              |Z|X|C|V|
+-+-+-+-+              +-+-+-+-+
Extra Directional Controls (For games like Space Invaders, Brix, Pong)Movement: Left Arrow / Right Arrow (Maps to CHIP-8 7 / 9 and 4 / 6)Up/Down: Up Arrow / Down Arrow (Maps to CHIP-8 1 / 4)Action / Shoot: Spacebar (Maps to CHIP-8 5 and 8)Quit Emulator: ESC
