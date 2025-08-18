#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>



enum Opcode : uint8_t {
    // Arithmetic (R-type: opcode, rd, rs1, rs2)
    OP_ADD  = 0x00, // R-type
    OP_SUB  = 0x01, // R-type
    OP_MUL  = 0x02, // R-type
    OP_DIV  = 0x03, // R-type
    OP_MOD  = 0x04, // R-type

    // Binary Logic (R-type)
    OP_AND  = 0x05, // R-type
    OP_OR   = 0x06, // R-type
    OP_XOR  = 0x07, // R-type
    OP_SHL  = 0x08, // R-type
    OP_SHR  = 0x09, // R-type

    // Immediate Arithmetic (I-type: opcode, rd, rs1, imm)
    OP_ADDI = 0x0A, // I-type
    OP_SUBI = 0x0B, // I-type
    OP_MULI = 0x0C, // I-type
    OP_ANDI = 0x0D, // I-type
    OP_ORI  = 0x0E, // I-type

    // Memory Operations (M-type: opcode, rd/rs, address)
    OP_LD   = 0x0F, // M-type
    OP_ST   = 0x10, // M-type

    // Control Flow
    OP_JMP  = 0x11, // J-type: opcode, address
    OP_BEQ  = 0x12, // B-type: opcode, rs1, rs2, offset
    OP_BNE  = 0x13, // B-type: opcode, rs1, rs2, offset
    OP_CALL = 0x14, // J-type: opcode, address
    OP_RET  = 0x15, // R-type (no operands, uses stack/PC)
    OP_HLT  = 0x16  // R-type (no operands)
};

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

class VM {
    uint32_t registers[8]; // registers
    uint32_t pc = 0; // program counter
    // stack pointer, starts at 0xFFFC 
    //since it is the last address divisible by 4
    uint32_t sp = 0xFFFC; 

    // 64 KiB of memory
    std::vector<uint8_t> memory = std::vector<uint8_t>(64 * 1024); // one byte in each space

    // vm function to load program into memory
    bool loadProgram(std::string& filename) {
        // Reset the VM state
        std::fill(std::begin(registers), std::end(registers), 0);
        pc = 0;
        sp: 0xFFFC;

        // clear ram
        std::fill(memory.begin(), memory.end(), 0);
        
        // read in the file
        // opens program file, reads in binary format to prevent any changes to file
        std::ifstream programFile(filename, std::ios::binary);

        if (!programFile) {
            throw std::runtime_error("Failed to open program file");
        }
        
        // range constructor to create buffer, takes in iterators first and last and creates a vector from [first, last)
        // std::istreambuf_iterator<char>(programFile) is an iterator pointing to the start of the programFile
        // std::istreambuf_iterator<char>() is the default-constructed iterator, which represents the end-of-stream marker
        std::vector<char> buffer(
            (std::istreambuf_iterator<char>(programFile)),
            std::istreambuf_iterator<char>()
        );

        if (buffer.size() > memory.size()) {
            throw std::runtime_error("Program is too large to fit in VM memory");
        }
        
        // copy from char buffer to memory
        // copy reinterprets each byte from char to uint_8 since both are 1-byte
        std::copy(buffer.begin(), buffer.end(), memory.begin());

    }

};

uint32_t getOpcode(uint32_t instructionCode) {
    uint32_t bitMask = 0b00111111;
    uint8_t opcode = (instructionCode >> 26) & bitMask;
    return opcode;
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