#include <iostream>
#include <vector>

struct VM {
    uint32_t registers[8]; // registers
    uint32_t pc = 0; // program counter
    // stack pointer, starts at 0xFFFC 
    //since it is the last address divisible by 4
    uint32_t sp = 0xFFFC; 

    // 64 KiB of memory
    std::vector<uint8_t> memory = std::vector<uint8_t>(64 * 1024); // one byte in each space
};

uint8_t getOpcode(uint32_t instruction) {
    uint32_t bitMask = 00000000000000000000000000111111;
    uint8_t opcode = instruction & bitMask;
    return opcode;
}
