cmake_minimum_required(VERSION 3.10)
project(Chip8Emulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} src)

add_executable(chip8_emulator
    src/main.cpp
    src/Chip8.cpp
)

target_link_libraries(chip8_emulator ${SDL2_LIBRARIES})
