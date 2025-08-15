#include <iostream>
#include <vector>

class Instruction {
    public:
        uint8_t opcode;

};

class RTypeInstruction: public Instruction {
    // Opcode(6) RD(4) RS1(4) RS2(4) Unused(14) 
    public:
        uint8_t rd, rs1, rs2;
};

class ITypeInstruction: public Instruction {
    // Opcode(6) RD(4) RS(4) IMM(16) Unused(2)
    public:
    uint8_t rd, rs;
    uint16_t imm;
};

class JTypeInstruction: public Instruction {
    // Opcode(6) Address(16) Unused(10)
    public:
    uint16_t adr;

};

class MTypeInstruction: public Instruction {
    // Opcode(6) RD(4) RS(4) OFFSET(16) Unused(2) 
    public:
    uint8_t rd, rs;
    uint16_t offset;

};

class BTypeInstruction: public Instruction {
    // Opcode(6) RS1(4) RS2(4) OFFSET(16) UNUSED(2)
    public:
    uint8_t rs1, rs2;
    uint16_t offset;
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

RTypeInstruction decodeRTypeInstruction(uint32_t instructionCode) {
    // R type instruction: Opcode(6) RD(4) RS1(4) RS2(4) Unused(14)
    uint32_t opcodeBitmask = 0x3F; // 0b00111111
    uint32_t bitmask = 0x0F; // 0b00001111
    RTypeInstruction instruction;
    instruction.opcode = (instructionCode >> 26) & opcodeBitmask;
    instruction.rd = (instructionCode >> 22) & bitmask;
    instruction.rs1 = (instructionCode >> 18) & bitmask;
    instruction.rs2 = (instructionCode >> 14) & bitmask;
    return instruction;
}

ITypeInstruction decodeITypeInstruction(uint32_t instructionCode) {
    // Opcode(6) RD(4) RS(4) IMM(16) Unused(2)
    ITypeInstruction instruction;
    instruction.opcode = (instructionCode >> 26) & 0x3F; // 0b00111111
    instruction.rd = (instructionCode >> 22) & 0x0F; // 0b00001111
    instruction.rs = (instructionCode >> 18) & 0x0F; // 0b00001111
    instruction.imm = (instructionCode >> 2) & 0xFFFF; // 0b1111111111111111

    return instruction;
}

JTypeInstruction decodeJTypeInstruction(uint32_t instructionCode) {
    // Opcode(6) Address(16) Unused(10)
    JTypeInstruction instruction;
    instruction.opcode = (instructionCode >> 26) & 0x3F; // 0b00111111
    instruction.adr = (instructionCode >> 10) & 0xFFFF; // 0b1111111111111111

    return instruction;
}

MTypeInstruction decodeMTypeInstruction(uint32_t instructionCode) {
    // Opcode(6) RD(4) RS(4) OFFSET(16) Unused(2) 
    MTypeInstruction instruction;
    instruction.opcode = (instructionCode >> 26) & 0x3F; // 0b00111111
    instruction.rd = (instructionCode >> 22) & 0x0F; // 0b00001111
    instruction.rs = (instructionCode >> 18) & 0x0F; // 0b00001111
    instruction.offset = (instructionCode >> 2) & 0xFFFF; // 0b1111111111111111

    return instruction;
}








