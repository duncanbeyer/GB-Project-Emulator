#pragma once

#include <common.h>

typedef enum { // Memory addressing modes
    A_IMP,
    A_R_D16,
    A_R_R,
    A_MR_R,
    A_R,
    A_R_D8,
    A_R_MR,
    A_R_HLI,
    A_R_HLD,
    A_HLI_R,
    A_HLD_R,
    A_R_A8,
    A_A8_R,
    A_HL_SPR,
    A_D16,
    A_D8,
    A_D16_R,
    A_MR_D8,
    A_MR,
    A_A16_R,
    A_R_A16
} addressing_mode;


typedef enum { // Register Types
    R_NONE,
    R_A,
    R_F,
    R_B,
    R_C,
    R_D,
    R_E,
    R_H,
    R_L,
    R_AF,
    R_BC,
    R_DE,
    R_HL,
    R_SP,
    R_PC
} register_type;


typedef enum { // Instruction Type
    I_NONE, // Null Instruction
    I_NOP, // No-Op
    I_LD, // Load Instruction
    I_INC, // Increment
    I_DEC, // Decrement
    I_RLCA,
    I_ADD, // Add
    I_RRCA,
    I_STOP, // Stop system
    I_RLA,
    I_JR, // Jump Relative
    I_RRA,
    I_DAA,
    I_CPL, // Complement accumulator (Flips all bits in A, sets n and h flags)
    I_SCF,
    I_CCF,
    I_HALT, // Halt system clock
    I_ADC, // Add with carry
    I_SUB, // Subtract
    I_SBC, // Subtract with carry
    I_AND, // Bitwise And
    I_XOR, // Bitwise Xor
    I_OR, // Bitwise Or
    I_CP, // Compare
    I_POP, // Pop from stack
    I_JP, // Jump
    I_PUSH, // Push to stack
    I_RET, // Return
    I_CB, // Goto CB
    I_CALL, // Call function
    I_RETI, // Return from interrupt
    I_LDH,
    I_JPHL,
    I_DI, // Disable Interrupts
    I_EI, // Enable Interrupts
    I_RST, // Restart
    I_ERR, 
    // CB Instructions 
    I_RLC,
    I_RRC,
    I_RL,
    I_RR,
    I_SLA,
    I_SRA,
    I_SWAP,
    I_SRL,
    I_BIT,
    I_RES,
    I_SET
} instruction_type;

typedef enum { // Used by conditionals
    C_NONE,
    C_NZ,
    C_Z,
    C_NC,
    C_C
} condition_type;

typedef struct { // Instruction structure, containing the above variable enums
    instruction_type type;
    addressing_mode mode;
    register_type reg1;
    register_type reg2;
    condition_type condition;
    u8 param;
} instruction;

instruction *opcode_to_instruction(u8 opcode);

char *inst_name(u8 t);