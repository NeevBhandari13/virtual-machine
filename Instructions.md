4 byte instructions (32 bit)

## Instruction Types

### R Type
Register Operations

| **Language** | **Format**                                                                                 |
| ------------ | ------------------------------------------------------------------------------------------ |
| Assembly     | (instruction) (destination register) (source register 1) (source register 2) (unused bits) |
| Bytecode     | Opcode(6) RD(4) RS1(4) RS2(4) Unused(14)                                                   |

### I Type
Immediate + Register Operations

| **Language** | **Format**                                                         |
| ------------ | ------------------------------------------------------------------ |
| Assembly     | (instruction) (destination register) (source register) (Immediate) |
| Bytecode     | Opcode(6) RD(4) RS(4) IMM(16) Unused(2)                            |
Large immediates will have to be loaded into a register
### J Type
Jump instructions to move around program

| **Language** | **Format**                       |
| ------------ | -------------------------------- |
| Assembly     | (instruction) (Address)          |
| Bytecode     | Opcode(6) Address(16) Unused(10) |
Larger addresses (not needed for 64KiB will need to be loaded from register)
### M Type
Memory operations

| **Language** | **Format**                                                      |
| ------------ | --------------------------------------------------------------- |
| Assembly     | (instruction) (Destination register) (Source Register) (Offset) |
| Bytecode     | Opcode(6) RD(4) RS(4) OFFSET(18)                                |
|              |                                                                 |
### B Type
Branching logic

| **Language** | **Format**                                                     |
| ------------ | -------------------------------------------------------------- |
| Assembly     | (instruction) (source register 1) (source register 2) (Offset) |
| Bytecode     | Opcode(6) RS1(4) RS2(4) OFFSET(18)                             |


## Instructions
### Arithmetic

| Mnemonic | Opcode (bin) | Opcode (hex) | Format | Operands       | Description      |
| -------- | ------------ | ------------ | ------ | -------------- | ---------------- |
| `ADD`    | `000000`     | `0x00`       | R      | `rd, rs1, rs2` | `rd ← rs1 + rs2` |
| `SUB`    | `000001`     | `0x01`       | R      | `rd, rs1, rs2` | `rd ← rs1 - rs2` |
| `MUL`    | `000010`     | `0x02`       | R      | `rd, rs1, rs2` | `rd ← rs1 * rs2` |
| `DIV`    | `000011`     | `0x03`       | R      | `rd, rs1, rs2` | `rd ← rs1 / rs2` |
| `MOD`    | `000100`     | `0x04`       | R      | `rd, rs1, rs2` | `rd ← rs1 % rs2` |
### Binary Logic

| Mnemonic | Opcode (bin) | Opcode (hex) | Format | Operands       | Description       |
| -------- | ------------ | ------------ | ------ | -------------- | ----------------- |
| `AND`    | `000101`     | `0x05`       | R      | `rd, rs1, rs2` | `rd ← rs1 & rs2`  |
| `OR`     | `000110`     | `0x06`       | R      | `rd, rs1, rs2` | `rd ← rs1 \| rs2` |
| `XOR`    | `000111`     | `0x07`       | R      | `rd, rs1, rs2` | `rd ← rs1 ^ rs2`  |
| `SHL`    | `001000`     | `0x08`       | R      | `rd, rs1, rs2` | `rd ← rs1 << rs2` |
| `SHR`    | `001001`     | `0x09`       | R      | `rd, rs1, rs2` | `rd ← rs1 >> rs2` |
### Immediate Arithmetic
|Mnemonic|Opcode (bin)|Opcode (hex)|Format|Operands|Description|
|---|---|---|---|---|---|
|`ADDI`|`001010`|`0x0A`|I|`rd, rs, imm`|`rd ← rs + imm`|
|`SUBI`|`001011`|`0x0B`|I|`rd, rs, imm`|`rd ← rs - imm`|
|`MULI`|`001100`|`0x0C`|I|`rd, rs, imm`|`rd ← rs * imm`|
|`ANDI`|`001101`|`0x0D`|I|`rd, rs, imm`|`rd ← rs & imm`|
|`ORI`|`001110`|`0x0E`|I|`rd, rs, imm`|`rd ← rs \| imm`|
### Memory Operations
|Mnemonic|Opcode (bin)|Opcode (hex)|Format|Operands|Description|
|---|---|---|---|---|---|
|`LD`|`001111`|`0x0F`|M|`rd, [rs + offset]`|`rd ← MEM[rs + offset]`|
|`ST`|`010000`|`0x10`|M|`[rs + offset], rt`|`MEM[rs + offset] ← rt`|
### Control Flow
| Mnemonic | Opcode (bin) | Opcode (hex) | Format | Operands         | Description                     |
| -------- | ------------ | ------------ | ------ | ---------------- | ------------------------------- |
| `JMP`    | `010001`     | `0x11`       | J      | `addr`           | `PC ← addr`                     |
| `BEQ`    | `010010`     | `0x12`       | B      | `r1, r2, offset` | `if r1 == r2: PC ← PC + offset` |
| `BNE`    | `010011`     | `0x13`       | B      | `r1, r2, offset` | `if r1 ≠ r2: PC ← PC + offset`  |
| `CALL`   | `010100`     | `0x14`       | J      | `addr`           | `Push PC+4; PC ← addr`          |
| `RET`    | `010101`     | `0x15`       | R      | none             | `PC ← Pop()`                    |
| `HLT`    | `010110`     | `0x16`       | R      | none             | Halt the VM                     |
