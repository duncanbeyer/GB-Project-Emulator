#include <instructions.h>
#include <cpu.h>
#include <bus.h>

// {Instruction, Addressing Mode, Registers/Values used}
instruction instructions[0x100] = {
    // 0x0
    [0x00] = {I_NOP, A_IMP},
    [0x01] = {I_LD, A_R_D16, R_BC},
    [0x02] = {I_LD, A_MR_R, R_BC, R_A},
    [0x03] = {I_INC, A_R, R_BC},
    [0x04] = {I_INC, A_R, R_B},
    [0x05] = {I_DEC, A_R, R_B},
    [0x06] = {I_LD, A_R_D8, R_B},
    [0x07] = {I_RLCA},
    [0x08] = {I_LD, A_A16_R, R_NONE, R_SP},
    [0x09] = {I_ADD, A_R_R, R_HL, R_BC},
    [0x0A] = {I_LD, A_R_MR, R_A, R_BC},
    [0x0B] = {I_DEC, A_R, R_BC},
    [0x0C] = {I_INC, A_R, R_C},
    [0x0D] = {I_DEC, A_R, R_C},
    [0x0E] = {I_LD, A_R_D8, R_C},
    [0x0F] = {I_RRCA},

    // 0x1

    [0x10] = {I_STOP},
    [0x11] = {I_LD, A_R_D16, R_DE},
    [0x12] = {I_LD, A_MR_R, R_DE, R_A},
    [0x13] = {I_INC, A_R, R_DE},
    [0x14] = {I_INC, A_R, R_D},
    [0x15] = {I_DEC, A_R, R_D},
    [0x16] = {I_LD, A_R_D8, R_D},
    [0x17] = {I_RLA},
    [0x18] = {I_JR, A_D8},
    [0x19] = {I_ADD, A_R_R, R_HL, R_DE},
    [0x1A] = {I_LD, A_R_MR, R_A, R_DE},
    [0x1B] = {I_DEC, A_R, R_DE},
    [0x1C] = {I_INC, A_R, R_E},
    [0x1D] = {I_DEC, A_R, R_E},
    [0x1E] = {I_LD, A_R_D8, R_E},
    [0x1F] = {I_RRA},

    // 0x

    [0x20] = {I_JR, A_D8, R_NONE, R_NONE, C_NZ},
    [0x21] = {I_LD, A_R_D16, R_HL},
    [0x22] = {I_LD, A_HLI_R, R_HL, R_A},
    [0x23] = {I_INC, A_R, R_HL},
    [0x24] = {I_INC, A_R, R_H},
    [0x25] = {I_DEC, A_R, R_H},
    [0x26] = {I_LD, A_R_D8, R_H},
    [0x27] = {I_DAA},
    [0x28] = {I_JR, A_D8, R_NONE, R_NONE, C_Z},
    [0x29] = {I_ADD, A_R_R, R_HL, R_HL},
    [0x2A] = {I_LD, A_R_HLI, R_A, R_HL},
    [0x2B] = {I_DEC, A_R, R_HL},
    [0x2C] = {I_INC, A_R, R_L},
    [0x2D] = {I_DEC, A_R, R_L},
    [0x2E] = {I_LD, A_R_D8, R_L},
    [0x2F] = {I_CPL},

    // 0x3

    [0x30] = {I_JR, A_D8, R_NONE, R_NONE, C_NC},
    [0x31] = {I_LD, A_R_D16, R_SP},
    [0x32] = {I_LD, A_HLD_R, R_HL, R_A},
    [0x33] = {I_INC, A_R, R_SP},
    [0x34] = {I_INC, A_MR, R_HL},
    [0x35] = {I_DEC, A_MR, R_HL},
    [0x36] = {I_LD, A_MR_D8, R_HL},
    [0x37] = {I_SCF},
    [0x38] = {I_JR, A_D8, R_NONE, R_NONE, C_C},
    [0x39] = {I_ADD, A_R_R, R_HL, R_SP},
    [0x3A] = {I_LD, A_R_HLD, R_A, R_HL},
    [0x3B] = {I_DEC, A_R, R_SP},
    [0x3C] = {I_INC, A_R, R_A},
    [0x3D] = {I_DEC, A_R, R_A},
    [0x3E] = {I_LD, A_R_D8, R_A},
    [0x3F] = {I_CCF},

    // 0x4

    [0x40] = {I_LD, A_R_R, R_B, R_B},
    [0x41] = {I_LD, A_R_R, R_B, R_C},
    [0x42] = {I_LD, A_R_R, R_B, R_D},
    [0x43] = {I_LD, A_R_R, R_B, R_E},
    [0x44] = {I_LD, A_R_R, R_B, R_H},
    [0x45] = {I_LD, A_R_R, R_B, R_L},
    [0x46] = {I_LD, A_R_MR, R_B, R_HL},
    [0x47] = {I_LD, A_R_R, R_B, R_A},
    [0x48] = {I_LD, A_R_R, R_C, R_B},
    [0x49] = {I_LD, A_R_R, R_C, R_C},
    [0x4A] = {I_LD, A_R_R, R_C, R_D},
    [0x4B] = {I_LD, A_R_R, R_C, R_E},
    [0x4C] = {I_LD, A_R_R, R_C, R_H},
    [0x4D] = {I_LD, A_R_R, R_C, R_L},
    [0x4E] = {I_LD, A_R_MR, R_C, R_HL},
    [0x4F] = {I_LD, A_R_R, R_C, R_A},

    // 0x5

    [0x50] = {I_LD, A_R_R, R_D, R_B},
    [0x51] = {I_LD, A_R_R, R_D, R_C},
    [0x52] = {I_LD, A_R_R, R_D, R_D},
    [0x53] = {I_LD, A_R_R, R_D, R_E},
    [0x54] = {I_LD, A_R_R, R_D, R_H},
    [0x55] = {I_LD, A_R_R, R_D, R_L},
    [0x56] = {I_LD, A_R_MR, R_D, R_HL},
    [0x57] = {I_LD, A_R_R, R_D, R_A},
    [0x58] = {I_LD, A_R_R, R_E, R_B},
    [0x59] = {I_LD, A_R_R, R_E, R_C},
    [0x5A] = {I_LD, A_R_R, R_E, R_D},
    [0x5B] = {I_LD, A_R_R, R_E, R_E},
    [0x5C] = {I_LD, A_R_R, R_E, R_H},
    [0x5D] = {I_LD, A_R_R, R_E, R_L},
    [0x5E] = {I_LD, A_R_MR, R_E, R_HL},
    [0x5F] = {I_LD, A_R_R, R_E, R_A},

    // 0x6

    [0x60] = {I_LD, A_R_R, R_H, R_B},
    [0x61] = {I_LD, A_R_R, R_H, R_C},
    [0x62] = {I_LD, A_R_R, R_H, R_D},
    [0x63] = {I_LD, A_R_R, R_H, R_E},
    [0x64] = {I_LD, A_R_R, R_H, R_H},
    [0x65] = {I_LD, A_R_R, R_H, R_L},
    [0x66] = {I_LD, A_R_MR, R_H, R_HL},
    [0x67] = {I_LD, A_R_R, R_H, R_A},
    [0x68] = {I_LD, A_R_R, R_L, R_B},
    [0x69] = {I_LD, A_R_R, R_L, R_C},
    [0x6A] = {I_LD, A_R_R, R_L, R_D},
    [0x6B] = {I_LD, A_R_R, R_L, R_E},
    [0x6C] = {I_LD, A_R_R, R_L, R_H},
    [0x6D] = {I_LD, A_R_R, R_L, R_L},
    [0x6E] = {I_LD, A_R_MR, R_L, R_HL},
    [0x6F] = {I_LD, A_R_R, R_L, R_A},

    // 0x7

    [0x70] = {I_LD, A_MR_R, R_HL, R_B},
    [0x71] = {I_LD, A_MR_R, R_HL, R_C},
    [0x72] = {I_LD, A_MR_R, R_HL, R_D},
    [0x73] = {I_LD, A_MR_R, R_HL, R_E},
    [0x74] = {I_LD, A_MR_R, R_HL, R_H},
    [0x75] = {I_LD, A_MR_R, R_HL, R_L},
    [0x76] = {I_HALT},
    [0x77] = {I_LD, A_MR_R, R_HL, R_A},
    [0x78] = {I_LD, A_R_R, R_A, R_B},
    [0x79] = {I_LD, A_R_R, R_A, R_C},
    [0x7A] = {I_LD, A_R_R, R_A, R_D},
    [0x7B] = {I_LD, A_R_R, R_A, R_E},
    [0x7C] = {I_LD, A_R_R, R_A, R_H},
    [0x7D] = {I_LD, A_R_R, R_A, R_L},
    [0x7E] = {I_LD, A_R_MR, R_A, R_HL},
    [0x7F] = {I_LD, A_R_R, R_A, R_A},

    // 0x8

    [0x80] = {I_ADD, A_R_R, R_A, R_B},
    [0x81] = {I_ADD, A_R_R, R_A, R_C},
    [0x82] = {I_ADD, A_R_R, R_A, R_D},
    [0x83] = {I_ADD, A_R_R, R_A, R_E},
    [0x84] = {I_ADD, A_R_R, R_A, R_H},
    [0x85] = {I_ADD, A_R_R, R_A, R_L},
    [0x86] = {I_ADD, A_R_MR, R_A, R_HL},
    [0x87] = {I_ADD, A_R_R, R_A, R_A},
    [0x88] = {I_ADC, A_R_R, R_A, R_B},
    [0x89] = {I_ADC, A_R_R, R_A, R_C},
    [0x8A] = {I_ADC, A_R_R, R_A, R_D},
    [0x8B] = {I_ADC, A_R_R, R_A, R_E},
    [0x8C] = {I_ADC, A_R_R, R_A, R_H},
    [0x8D] = {I_ADC, A_R_R, R_A, R_L},
    [0x8E] = {I_ADC, A_R_MR, R_A, R_HL},
    [0x8F] = {I_ADC, A_R_R, R_A, R_A},

    // 0x9

    [0x90] = {I_SUB, A_R_R, R_A, R_B},
    [0x91] = {I_SUB, A_R_R, R_A, R_C},
    [0x92] = {I_SUB, A_R_R, R_A, R_D},
    [0x93] = {I_SUB, A_R_R, R_A, R_E},
    [0x94] = {I_SUB, A_R_R, R_A, R_H},
    [0x95] = {I_SUB, A_R_R, R_A, R_L},
    [0x96] = {I_SUB, A_R_MR, R_A, R_HL},
    [0x97] = {I_SUB, A_R_R, R_A, R_A},
    [0x98] = {I_SBC, A_R_R, R_A, R_B},
    [0x99] = {I_SBC, A_R_R, R_A, R_C},
    [0x9A] = {I_SBC, A_R_R, R_A, R_D},
    [0x9B] = {I_SBC, A_R_R, R_A, R_E},
    [0x9C] = {I_SBC, A_R_R, R_A, R_H},
    [0x9D] = {I_SBC, A_R_R, R_A, R_L},
    [0x9E] = {I_SBC, A_R_MR, R_A, R_HL},
    [0x9F] = {I_SBC, A_R_R, R_A, R_A},

    // 0xA

    [0xA0] = {I_AND, A_R_R, R_A, R_B},
    [0xA1] = {I_AND, A_R_R, R_A, R_C},
    [0xA2] = {I_AND, A_R_R, R_A, R_D},
    [0xA3] = {I_AND, A_R_R, R_A, R_E},
    [0xA4] = {I_AND, A_R_R, R_A, R_H},
    [0xA5] = {I_AND, A_R_R, R_A, R_L},
    [0xA6] = {I_AND, A_R_MR, R_A, R_HL},
    [0xA7] = {I_AND, A_R_R, R_A, R_A},
    [0xA8] = {I_XOR, A_R_R, R_A, R_B},
    [0xA9] = {I_XOR, A_R_R, R_A, R_C},
    [0xAA] = {I_XOR, A_R_R, R_A, R_D},
    [0xAB] = {I_XOR, A_R_R, R_A, R_E},
    [0xAC] = {I_XOR, A_R_R, R_A, R_H},
    [0xAD] = {I_XOR, A_R_R, R_A, R_L},
    [0xAE] = {I_XOR, A_R_MR, R_A, R_HL},
    [0xAF] = {I_XOR, A_R_R, R_A, R_A},

    // 0xB

    [0xB0] = {I_OR, A_R_R, R_A, R_B},
    [0xB1] = {I_OR, A_R_R, R_A, R_C},
    [0xB2] = {I_OR, A_R_R, R_A, R_D},
    [0xB3] = {I_OR, A_R_R, R_A, R_E},
    [0xB4] = {I_OR, A_R_R, R_A, R_H},
    [0xB5] = {I_OR, A_R_R, R_A, R_L},
    [0xB6] = {I_OR, A_R_MR, R_A, R_HL},
    [0xB7] = {I_OR, A_R_R, R_A, R_A},
    [0xB8] = {I_CP, A_R_R, R_A, R_B},
    [0xB9] = {I_CP, A_R_R, R_A, R_C},
    [0xBA] = {I_CP, A_R_R, R_A, R_D},
    [0xBB] = {I_CP, A_R_R, R_A, R_E},
    [0xBC] = {I_CP, A_R_R, R_A, R_H},
    [0xBD] = {I_CP, A_R_R, R_A, R_L},
    [0xBE] = {I_CP, A_R_MR, R_A, R_HL},
    [0xBF] = {I_CP, A_R_R, R_A, R_A},

    // 0xC

    [0xC0] = {I_RET, A_IMP, R_NONE, R_NONE, C_NZ},
    [0xC1] = {I_POP, A_R, R_BC},
    [0xC2] = {I_JP, A_D16, R_NONE, R_NONE, C_NZ},
    [0xC3] = {I_JP, A_D16},
    [0xC4] = {I_CALL, A_D16, R_NONE, R_NONE, C_NZ}, 
    [0xC5] = {I_PUSH, A_R, R_BC},
    [0xC6] = {I_ADD, A_R_D8, R_A},
    [0xC7] = {I_RST, A_IMP, R_NONE, R_NONE, C_NONE, 0x00},
    [0xC8] = {I_RET, A_IMP, R_NONE, R_NONE, C_Z},
    [0xC9] = {I_RET},
    [0xCA] = {I_JP, A_D16, R_NONE, R_NONE, C_Z}, 
    [0xCB] = {I_CB, A_D8},
    [0xCC] = {I_CALL, A_D16, R_NONE, R_NONE, C_Z}, 
    [0xCD] = {I_CALL, A_D16},
    [0xCE] = {I_ADC, A_R_D8, R_A},
    [0xCF] = {I_RST, A_IMP, R_NONE, R_NONE, C_NONE, 0x08},

    // 0xD

    [0xD0] = {I_RET, A_IMP, R_NONE, R_NONE, C_NC},
    [0xD1] = {I_POP, A_R, R_DE},
    [0xD2] = {I_JP, A_D16, R_NONE, R_NONE, C_NC},
    [0xD4] = {I_CALL, A_D16, R_NONE, R_NONE, C_NC}, 
    [0xD5] = {I_PUSH, A_R, R_DE},
    [0xD6] = {I_SUB, A_R_D8, R_A},
    [0xD7] = {I_RST, A_IMP, R_NONE, R_NONE, C_NONE, 0x10},
    [0xD8] = {I_RET, A_IMP, R_NONE, R_NONE, C_C},
    [0xD9] = {I_RETI},
    [0xDA] = {I_JP, A_D16, R_NONE, R_NONE, C_C},
    [0xDC] = {I_CALL, A_D16, R_NONE, R_NONE, C_C},
    [0xDE] = {I_SBC, A_R_D8, R_A},
    [0xDF] = {I_RST, A_IMP, R_NONE, R_NONE, C_NONE, 0x18},

    // 0xE

    [0xE0] = {I_LDH, A_A8_R, R_NONE, R_A},
    [0xE1] = {I_POP, A_R, R_HL},
    [0xE2] = {I_LD, A_MR_R, R_C, R_A},
    [0xE5] = {I_PUSH, A_R, R_HL},
    [0xE6] = {I_AND, A_R_D8, R_A},
    [0xE7] = {I_RST, A_IMP, R_NONE, R_NONE, C_NONE, 0x20},
    [0xE8] = {I_ADD, A_R_D8, R_SP},
    [0xE9] = {I_JP, A_R, R_HL},
    [0xEA] = {I_LD, A_A16_R, R_NONE, R_A},
    [0xEE] = {I_XOR, A_R_D8, R_A},
    [0xEF] = {I_RST, A_IMP, R_NONE, R_NONE, C_NONE, 0x28},
    // 0xF

    [0xF0] = {I_LDH, A_R_A8, R_A},
    [0xF1] = {I_POP, A_R, R_AF},
    [0xF2] = {I_LD, A_R_MR, R_A, R_C},
    [0xF3] = {I_DI},
    [0xF5] = {I_PUSH, A_R, R_AF},
    [0xF6] = {I_OR, A_R_D8, R_A},
    [0xF7] = {I_RST, A_IMP, R_NONE, R_NONE, C_NONE, 0x30},
    [0xF8] = {I_LD, A_HL_SPR, R_HL, R_SP},
    [0xF9] = {I_LD, A_R_R, R_SP, R_HL},
    [0xFA] = {I_LD, A_R_A16, R_A},
    [0xFB] = {I_EI},
    [0xFE] = {I_CP, A_R_D8, R_A},
    [0xFF] = {I_RST, A_IMP, R_NONE, R_NONE, C_NONE, 0x38}
};

char *inst_lookup[] = {
    "<NONE>",
    "NOP",
    "LD",
    "INC",
    "DEC",
    "RLCA",
    "ADD",
    "RRCA",
    "STOP",
    "RLA",
    "JR",
    "RRA",
    "DAA",
    "CPL",
    "SCF",
    "CCF",
    "HALT",
    "ADC",
    "SUB",
    "SBC",
    "AND",
    "XOR",
    "OR",
    "CP",
    "POP",
    "JP",
    "PUSH",
    "RET",
    "CB",
    "CALL",
    "RETI",
    "LDH",
    "JPHL",
    "DI",
    "EI",
    "RST",
    "IN_ERR",
    "IN_RLC", 
    "IN_RRC",
    "IN_RL", 
    "IN_RR",
    "IN_SLA", 
    "IN_SRA",
    "IN_SWAP", 
    "IN_SRL",
    "IN_BIT", 
    "IN_RES", 
    "IN_SET"
};


char* instructions2[] = {
    // 0x0
    "NOP", "LD BC,d16", "LD (BC),A", "INC BC", "INC B", "DEC B", "LD B,d8", "RLCA",
    "LD (a16),SP", "ADD HL,BC", "LD A,(BC)", "DEC BC", "INC C", "DEC C", "LD C,d8", "RRCA",
    // 0x1
    "STOP 0", "LD DE,d16", "LD (DE),A", "INC DE", "INC D", "DEC D", "LD D,d8", "RLA",
    "JR r8", "ADD HL,DE", "LD A,(DE)", "DEC DE", "INC E", "DEC E", "LD E,d8", "RRA",
    // 0x2
    "JR NZ,r8", "LD HL,d16", "LD (HL+),A", "INC HL", "INC H", "DEC H", "LD H,d8", "DAA",
    "JR Z,r8", "ADD HL,HL", "LD A,(HL+)", "DEC HL", "INC L", "DEC L", "LD L,d8", "CPL",
    // 0x3
    "JR NC,r8", "LD SP,d16", "LD (HL-),A", "INC SP", "INC (HL)", "DEC (HL)", "LD (HL),d8", "SCF",
    "JR C,r8", "ADD HL,SP", "LD A,(HL-)", "DEC SP", "INC A", "DEC A", "LD A,d8", "CCF",
    // 0x4
    "LD B,B", "LD B,C", "LD B,D", "LD B,E", "LD B,H", "LD B,L", "LD B,(HL)", "LD B,A",
    "LD C,B", "LD C,C", "LD C,D", "LD C,E", "LD C,H", "LD C,L", "LD C,(HL)", "LD C,A",
    // 0x5
    "LD D,B", "LD D,C", "LD D,D", "LD D,E", "LD D,H", "LD D,L", "LD D,(HL)", "LD D,A",
    "LD E,B", "LD E,C", "LD E,D", "LD E,E", "LD E,H", "LD E,L", "LD E,(HL)", "LD E,A",
    // 0x6
    "LD H,B", "LD H,C", "LD H,D", "LD H,E", "LD H,H", "LD H,L", "LD H,(HL)", "LD H,A",
    "LD L,B", "LD L,C", "LD L,D", "LD L,E", "LD L,H", "LD L,L", "LD L,(HL)", "LD L,A",
    // 0x7
    "LD (HL),B", "LD (HL),C", "LD (HL),D", "LD (HL),E", "LD (HL),H", "LD (HL),L", "HALT", "LD (HL),A",
    "LD A,B", "LD A,C", "LD A,D", "LD A,E", "LD A,H", "LD A,L", "LD A,(HL)", "LD A,A",
    // 0x8
    "ADD A,B", "ADD A,C", "ADD A,D", "ADD A,E", "ADD A,H", "ADD A,L", "ADD A,(HL)", "ADD A,A",
    "ADC A,B", "ADC A,C", "ADC A,D", "ADC A,E", "ADC A,H", "ADC A,L", "ADC A,(HL)", "ADC A,A",
    // 0x9
    "SUB B", "SUB C", "SUB D", "SUB E", "SUB H", "SUB L", "SUB (HL)", "SUB A",
    "SBC A,B", "SBC A,C", "SBC A,D", "SBC A,E", "SBC A,H", "SBC A,L", "SBC A,(HL)", "SBC A,A",
    // 0xA
    "AND B", "AND C", "AND D", "AND E", "AND H", "AND L", "AND (HL)", "AND A",
    "XOR B", "XOR C", "XOR D", "XOR E", "XOR H", "XOR L", "XOR (HL)", "XOR A",
    // 0xB
    "OR B", "OR C", "OR D", "OR E", "OR H", "OR L", "OR (HL)", "OR A",
    "CP B", "CP C", "CP D", "CP E", "CP H", "CP L", "CP (HL)", "CP A",
    // 0xC
    "RET NZ", "POP BC", "JP NZ,a16", "JP a16", "CALL NZ,a16", "PUSH BC", "ADD A,d8", "RST 00H",
    "RET Z", "RET", "JP Z,a16", "PREFIX CB", "CALL Z,a16", "CALL a16", "ADC A,d8", "RST 08H",
    // 0xD
    "RET NC", "POP DE", "JP NC,a16", "", "CALL NC,a16", "PUSH DE", "SUB d8", "RST 10H",
    "RET C", "RETI", "JP C,a16", "", "CALL C,a16", "", "SBC A,d8", "RST 18H",
    // 0xE
    "LDH (a8),A", "POP HL", "LD (C),A", "", "", "PUSH HL", "AND d8", "RST 20H",
    "ADD SP,r8", "JP (HL)", "LD (a16),A", "", "", "", "XOR d8", "RST 28H",
    // 0xF
    "LDH A,(a8)", "POP AF", "LD A,(C)", "DI", "", "PUSH AF", "OR d8", "RST 30H",
    "LD HL,SP+r8", "LD SP,HL", "LD A,(a16)", "EI", "", "", "CP d8", "RST 38H"
};



char *inst_name(u8 t) {
    return inst_lookup[t];
}


instruction *opcode_to_instruction(u8 opcode) {
    if (opcode == 0xE3) {
        printf("looking for instruction %04X\n", opcode);
    }
    return &instructions[opcode];
}


static char *rt_lookup[] = {
    "<NONE>",
    "A",
    "F",
    "B",
    "C",
    "D",
    "E",
    "H",
    "L",
    "AF",
    "BC",
    "DE",
    "HL",
    "SP",
    "PC"
};

void inst_to_str(cpu_context *ctx, char *str) {
    instruction *inst = ctx->current_instruction;
    sprintf(str, "%s ", inst_name(inst->type));

    switch(inst->mode) {
        case A_IMP:
            return;

        case A_R_D16:
        case A_R_A16:
            sprintf(str, "%s %s,$%04X", inst_name(inst->type), 
                rt_lookup[inst->reg1], ctx->fetch_data);
            return;

        case A_R:
            sprintf(str, "%s %s", inst_name(inst->type), 
                rt_lookup[inst->reg1]);
            return;

        case A_R_R: 
            sprintf(str, "%s %s,%s", inst_name(inst->type), 
                rt_lookup[inst->reg1], rt_lookup[inst->reg2]);
            return;

        case A_MR_R:
            sprintf(str, "%s (%s),%s", inst_name(inst->type), 
                rt_lookup[inst->reg1], rt_lookup[inst->reg2]);
            return;

        case A_MR:
            sprintf(str, "%s (%s)", inst_name(inst->type), 
                rt_lookup[inst->reg1]);
            return;

        case A_R_MR:
            sprintf(str, "%s %s,(%s)", inst_name(inst->type), 
                rt_lookup[inst->reg1], rt_lookup[inst->reg2]);
            return;

        case A_R_D8:
        case A_R_A8:
            sprintf(str, "%s %s,$%02X", inst_name(inst->type), 
                rt_lookup[inst->reg1], ctx->fetch_data & 0xFF);
            return;

        case A_R_HLI:
            sprintf(str, "%s %s,(%s+)", inst_name(inst->type), 
                rt_lookup[inst->reg1], rt_lookup[inst->reg2]);
            return;

        case A_R_HLD:
            sprintf(str, "%s %s,(%s-)", inst_name(inst->type), 
                rt_lookup[inst->reg1], rt_lookup[inst->reg2]);
            return;

        case A_HLI_R:
            sprintf(str, "%s (%s+),%s", inst_name(inst->type), 
                rt_lookup[inst->reg1], rt_lookup[inst->reg2]);
            return;

        case A_HLD_R:
            sprintf(str, "%s (%s-),%s", inst_name(inst->type), 
                rt_lookup[inst->reg1], rt_lookup[inst->reg2]);
            return;

        case A_A8_R:
            sprintf(str, "%s $%02X,%s", inst_name(inst->type), 
                bus_read(ctx->regs.pc - 1), rt_lookup[inst->reg2]);

            return;

        case A_HL_SPR:
            sprintf(str, "%s (%s),SP+%d", inst_name(inst->type), 
                rt_lookup[inst->reg1], ctx->fetch_data & 0xFF);
            return;

        case A_D8:
            sprintf(str, "%s $%02X", inst_name(inst->type), 
                ctx->fetch_data & 0xFF);
            return;

        case A_D16:
            sprintf(str, "%s $%04X", inst_name(inst->type), 
                ctx->fetch_data);
            return;

        case A_MR_D8:
            sprintf(str, "%s (%s),$%02X", inst_name(inst->type), 
                rt_lookup[inst->reg1], ctx->fetch_data & 0xFF);
            return;

        case A_A16_R:
            sprintf(str, "%s ($%04X),%s", inst_name(inst->type), 
                ctx->fetch_data, rt_lookup[inst->reg2]);
            return;

        default:
            fprintf(stderr, "INVALID AM: %d\n", inst->mode);
            NO_IMPL
    }
}