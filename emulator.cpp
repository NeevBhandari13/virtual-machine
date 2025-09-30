#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <unordered_map>


// enum for different instructionTypes
enum class InstructionType {
    R,
    I,
    J,
    B,
    M
};

enum Opcode : uint8_t {
    // Arithmetic (R-type: opcode, rd, rs1, rs2)
    OP_ADD  = 0x00, // R-type
    OP_SUB  = 0x01, // R-type
    OP_MUL  = 0x02, // R-type
    OP_DIV  = 0x03, // R-type
    OP_MOD  = 0x04, // R-type
    OP_MOV  = 0x18, // R-type

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
    OP_XORI = 0x0F, // I-type 
    OP_MOVI = 0x19, // I-type (only rd and imm)

    // Memory Operations (M-type: opcode, rd/rs, address)
    OP_LD   = 0x10, // M-type
    OP_ST   = 0x11, // M-type
    
    // Control Flow
    OP_JMP  = 0x12, // J-type: opcode, address
    OP_BEQ  = 0x13, // B-type: opcode, rs1, rs2, offset
    OP_BNE  = 0x14, // B-type: opcode, rs1, rs2, offset
    OP_CALL = 0x15, // J-type: opcode, address
    OP_RET  = 0x16, // R-type (no operands, uses stack/PC)
    OP_HLT  = 0x17  // R-type (no operands)
};


std::unordered_map<uint8_t, InstructionType> opcodeType = {
    // R-type
    {0x00, InstructionType::R}, // ADD
    {0x01, InstructionType::R}, // SUB
    {0x02, InstructionType::R}, // MUL
    {0x03, InstructionType::R}, // DIV
    {0x04, InstructionType::R}, // MOD
    {0x05, InstructionType::R}, // AND
    {0x06, InstructionType::R}, // OR
    {0x07, InstructionType::R}, // XOR
    {0x08, InstructionType::R}, // SHL
    {0x09, InstructionType::R}, // SHR
    {0x18, InstructionType::R}, // MOV

    // I-type
    {0x0A, InstructionType::I}, // ADDI
    {0x0B, InstructionType::I}, // SUBI
    {0x0C, InstructionType::I}, // MULI
    {0x0D, InstructionType::I}, // ANDI
    {0x0E, InstructionType::I}, // ORI
    {0x0F, InstructionType::I}, // XORI
    {0x19, InstructionType::I}, // MOVI

    // M-type
    {0x10, InstructionType::M}, // LD
    {0x11, InstructionType::M}, // ST

    // J-type
    {0x12, InstructionType::J}, // JMP
    {0x15, InstructionType::J}, // CALL
    {0x16, InstructionType::J}, // RET
    {0x17, InstructionType::J},  // HLT

    // B-Type
    {0x13, InstructionType::I}, // BEQ
    {0x14, InstructionType::I}, // BNE
};


// single instruction class with all fields
class Instruction {
    public:
        uint8_t opcode, rd, rs1, rs2;
        uint16_t imm, addr, offset;
};


uint32_t getOpcode(uint32_t instructionCode) {
    uint32_t bitMask = 0b00111111;
    uint8_t opcode = (instructionCode >> 26) & bitMask;
    std::cout << "instruction code shifted: " << (instructionCode >> 26) << " and masked: " << ((instructionCode >> 26) & bitMask) << std::endl;
    return opcode;
}

Instruction decodeRTypeInstruction(uint32_t instructionCode) {
    // R type instruction: Opcode(6) RD(4) RS1(4) RS2(4) Unused(14)
    uint32_t opcodeBitmask = 0x3F; // 0b00111111
    uint32_t bitmask = 0x0F; // 0b00001111
    Instruction instruction;
    instruction.opcode = (instructionCode >> 26) & opcodeBitmask;
    instruction.rd = (instructionCode >> 22) & bitmask;
    instruction.rs1 = (instructionCode >> 18) & bitmask;
    instruction.rs2 = (instructionCode >> 14) & bitmask;
    return instruction;
}

Instruction decodeITypeInstruction(uint32_t instructionCode) {
    // Opcode(6) RD(4) RS(4) IMM(16) Unused(2)
    Instruction instruction;
    instruction.opcode = (instructionCode >> 26) & 0x3F; // 0b00111111
    instruction.rd = (instructionCode >> 22) & 0x0F; // 0b00001111
    instruction.rs1 = (instructionCode >> 18) & 0x0F; // 0b00001111
    instruction.imm = (instructionCode >> 2) & 0xFFFF; // 0b1111111111111111

    return instruction;
}

Instruction decodeJTypeInstruction(uint32_t instructionCode) {
    // Opcode(6) Address(16) Unused(10)
    Instruction instruction;
    instruction.opcode = (instructionCode >> 26) & 0x3F; // 0b00111111
    instruction.addr = (instructionCode >> 10) & 0xFFFF; // 0b1111111111111111

    return instruction;
}

Instruction decodeBTypeInstruction(uint32_t instructionCode) {
    // Opcode(6) RS1(4) RS2(4) OFFSET(16) UNUSED(2)
    Instruction instruction;
    instruction.opcode = (instructionCode >> 26) & 0x3F; // 0b00111111
    instruction.rs1 = (instructionCode >> 22) & 0x0F; // 0b00001111
    instruction.rs2 = (instructionCode >> 18) & 0x0F; // 0b00001111
    instruction.offset = (instructionCode >> 2) & 0xFFFF; // 0b1111111111111111

    return instruction;
}

Instruction decodeMTypeInstruction(uint32_t instructionCode) {
    // Opcode(6) RD(4) RS(4) addr(16) Unused(2) 
    Instruction instruction;
    instruction.opcode = (instructionCode >> 26) & 0x3F; // 0b00111111
    instruction.rd = (instructionCode >> 22) & 0x0F; // 0b00001111
    instruction.rs1 = (instructionCode >> 18) & 0x0F; // 0b00001111
    instruction.addr = (instructionCode >> 2) & 0xFFFF; // 0b1111111111111111

    return instruction;
}

class VM {
    uint32_t registers[8]; // registers
    // 
    uint32_t pc = 0; // program counter
    // stack pointer, starts at 0xFFFC since we cant start at 0x10000, if we start at 0xFFFF, it will stay empty and words will be misaligned
    uint32_t sp = 0xFFF0; // sp points to current top of stack

    uint32_t consoleOutAddress = 0xFFFC; // memory address to write out to console
    bool halted = false;
    // 64 KiB of memory
    std::vector<uint8_t> memory = std::vector<uint8_t>(64 * 1024); // one byte in each space
    
    public: 

        // vm function to load program into memory
        void loadProgram(std::string& filename) {
            // Reset the VM state
            std::fill(std::begin(registers), std::end(registers), 0);
            pc = 0;
            sp = 0xFFFC;

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
            
        uint32_t readMemory(uint32_t addr) {
            if (addr + 3 >= memory.size()) {
                throw std::out_of_range("Memory read out of bounds");
            }
            // std::cout 
            //     << int(memory[addr+3]) << " "
            //     << int(memory[addr+2]) << " "
            //     << int(memory[addr+1]) << " "
            //     << int(memory[addr]) << std::endl;

            // Little-endian load: lowest byte first
            uint32_t word = (uint32_t(memory[addr+3]) << 24) |
            (uint32_t(memory[addr+2]) << 16) |
            (uint32_t(memory[addr+1]) << 8)  |
            uint32_t(memory[addr]);
            
            return word;
        }

        void writeMemory(uint32_t addr, uint32_t value) {
            std::cout << addr << '\n';
            if (addr == consoleOutAddress) {
                std::cout << (value & 0xFF); // only print one byte
            }

            if (addr + 3 >= memory.size()) {
                throw std::out_of_range("Memory write out of bounds");
            }

            // Little-endian store: lowest byte first
            memory[addr + 3] = (value >> 24) & 0xFF;
            memory[addr + 2] = (value >> 16) & 0xFF;
            memory[addr + 1] = (value >> 8) & 0xFF;
            memory[addr]     = value & 0xFF;
        }

        void pushStack(uint32_t value) {
            // stack goes from high addresses to low
            if (sp < 4) {
                throw std::overflow_error("Stack overflow");
            }

            sp -= 4;  // move stack pointer down
            writeMemory(sp, value);
        }   

        uint32_t popStack() {
            if (sp + 4 > memory.size()) {
                throw std::underflow_error("Stack underflow");
            }

            uint32_t value = readMemory(sp);
            sp += 4;  // move stack pointer up
            return value;
        }

        // used to check if we do not need to increment pc
        bool isJType(uint8_t opcode) {
            return opcodeType[opcode] == InstructionType::J;
        }
        bool isBType(uint8_t opcode) {
            return opcodeType[opcode] == InstructionType::B;
        }

        uint32_t fetchInstruction() {
            uint32_t instruction = readMemory(pc);
            return instruction;
        }

        Instruction decodeInstruction(uint32_t instCode) {
            uint8_t opcode = getOpcode(instCode);

            InstructionType type = opcodeType[opcode]; // look up the type

            switch (type) {
                case InstructionType::R:
                    return decodeRTypeInstruction(instCode);
                case InstructionType::I:
                    return decodeITypeInstruction(instCode);
                case InstructionType::M:
                    return decodeMTypeInstruction(instCode);
                case InstructionType::J:
                    return decodeJTypeInstruction(instCode);
                }
            throw std::runtime_error("Unknown instruction type");
        }

        void executeInstruction(const Instruction& inst) {
        switch(inst.opcode) {
            // =====================
            // R-TYPE
            // =====================
            case OP_ADD:
                registers[inst.rd] = registers[inst.rs1] + registers[inst.rs2];
                break;
            case OP_SUB:
                registers[inst.rd] = registers[inst.rs1] - registers[inst.rs2];
                break;
            case OP_MUL:
                registers[inst.rd] = registers[inst.rs1] * registers[inst.rs2];
                break;
            case OP_DIV:
                registers[inst.rd] = registers[inst.rs1] / registers[inst.rs2];
                break;
            case OP_MOD:
                registers[inst.rd] = registers[inst.rs1] % registers[inst.rs2];
                break;
            case OP_AND:
                registers[inst.rd] = registers[inst.rs1] & registers[inst.rs2];
                break;
            case OP_OR:
                registers[inst.rd] = registers[inst.rs1] | registers[inst.rs2];
                break;
            case OP_XOR:
                registers[inst.rd] = registers[inst.rs1] ^ registers[inst.rs2];
                break;
            case OP_SHL:
                registers[inst.rd] = registers[inst.rs1] << registers[inst.rs2];
                break;
            case OP_SHR:
                registers[inst.rd] = registers[inst.rs1] >> registers[inst.rs2];
                break;

            // =====================
            // I-TYPE
            // =====================
            case OP_ADDI:
                registers[inst.rd] = registers[inst.rs1] + inst.imm;
                break;
            case OP_SUBI:
                registers[inst.rd] = registers[inst.rs1] - inst.imm;
                break;
            case OP_ANDI:
                registers[inst.rd] = registers[inst.rs1] & inst.imm;
                break;
            case OP_ORI:
                registers[inst.rd] = registers[inst.rs1] | inst.imm;
                break;
            case OP_XORI:
                registers[inst.rd] = registers[inst.rs1] ^ inst.imm;
                break;

            // =====================
            // M-TYPE
            // =====================
            case OP_LD:
                registers[inst.rs1] = readMemory(inst.addr);
                break;
            case OP_ST:
                writeMemory(inst.addr, registers[inst.rs1]);
                break;

            // =====================
            // J-TYPE
            // =====================
            case OP_JMP:
                pc = inst.addr;
                break;
            case OP_CALL:
                pushStack(pc);
                pc = inst.addr;
                break;
            case OP_RET:
                pc = popStack();
                break;
            case OP_HLT:
                halted = true;
                break;

            case OP_BEQ:
                if (registers[inst.rs1] == registers[inst.rs2]) {
                    pc = pc + inst.offset;
                }

            case OP_MOV:
                registers[inst.rd] = registers[inst.rs1];
            case OP_MOVI:
                registers[inst.rd] = inst.imm;

            default:
                throw std::runtime_error("Unknown opcode: " + std::to_string(inst.opcode));
        }
}


    void run(std::string& fileName) {
        loadProgram(fileName);
        std::cout << "test1" << std::endl;
        while (!halted && pc < 32) {
            uint32_t instCode = fetchInstruction();
            Instruction inst = decodeInstruction(instCode);
            executeInstruction(inst);
            // don't update pc if we have halted or just jumped
            if (!halted && !isJType(inst.opcode) && !isBType(inst.opcode)) {
                pc += 4; // increment program counter
            }
            
        }
        std::cout << "test2" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: emulator <program.bin>\n";
        return 1;
    }

    std::string fileName = argv[1];

    try {
        VM vm;
        vm.run(fileName);
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
