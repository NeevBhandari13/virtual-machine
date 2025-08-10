#include <iostream>
#include <vector>

struct Instruction {
    uint8_t opcode;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint32_t immediate;
};

struct VM {
    uint32_t registers[8]; // registers
    uint32_t pc = 0; // program counter
    // stack pointer, starts at 0xFFFC 
    //since it is the last address divisible by 4
    uint32_t sp = 0xFFFC; 

    // 64 KiB of memory
    std::vector<uint8_t> memory = std::vector<uint8_t>(64 * 1024); // one byte in each space

    // vm function to process instruction
    void processInstruction(uint32_t instruction) {}
};

Instruction getOpcode(uint32_t instructionCode) {
    uint32_t bitMask = 0b00111111;
    uint8_t opcode = (instructionCode >> 26) & bitMask;
    Instruction instruction;
    instruction.opcode = opcode;
    return instruction;
}

Instruction decodeRTypeInstruction(Instruction instruction, uint32_t instructionCode) {
    // R type instruction: Opcode(6) RD(4) RS1(4) RS2(4) Unused(14)
    uint32_t bitMask = 0x0F; // 0b00001111
    instruction.rd = (instructionCode >> 22) & bitMask;
    instruction.rs1 = (instructionCode >> 18) & bitMask;
    instruction.rs2 = (instructionCode >> 14) & bitMask;
    return instruction;
}




