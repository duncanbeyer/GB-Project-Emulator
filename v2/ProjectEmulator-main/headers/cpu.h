#pragma once

#include <common.h>
#include <instructions.h>


typedef struct {  //  CPU REGISTER VARS
    u8 a; // Accumulator
    u8 f; // Flag
    u8 b; 
    u8 c;
    u8 d;
    u8 e;
    u8 h;
    u8 l;
    u16 sp;
    u16 pc;
} cpu_registers;


typedef struct {
    cpu_registers regs;
    u16 fetch_data; // currently used data
    u16 memory_destination;
    bool dest_is_mem;
    u8 current_opcode;
    instruction *current_instruction;

    bool paused;
    bool stepping;

    bool int_master_enabled;
    bool enabling_ime;

    u8 ie_register;
    u8 int_flags;
} cpu_context;


cpu_registers *cpu_get_regs();

void cpu_init();
bool cpu_step();

typedef void (*IN_PROC)(cpu_context *);

IN_PROC inst_get_processor(instruction_type x);

#define CPU_FLAG_Z BIT(ctx->regs.f, 7) // Zero flag is bit 7 of the flags register
#define CPU_FLAG_N BIT(ctx->regs.f, 6) // Carry flag is bit 4 of the flags register
#define CPU_FLAG_H BIT(ctx->regs.f, 5) // Carry flag is bit 4 of the flags register
#define CPU_FLAG_C BIT(ctx->regs.f, 4) // Carry flag is bit 4 of the flags register

u16 cpu_read_reg(register_type rt);
void cpu_set_reg(register_type rt, u16 val);

u8 cpu_get_ie_reg();
void cpu_set_ie_reg(u8 value);

u8 cpu_read_reg8(register_type rt);
void cpu_set_reg8(register_type rt, u8 val);

u8 cpu_get_int_flags();
void cpu_set_int_flags(u8 val);

/*
CPU Flags:

Bit 7: Z (Zero Flag)
This flag is usually set when the result of an operation is 0.

Bit 6: N (Add/Subtract Flag) (BCD)
This flag indicates whether the most recent operation was addition or subtraction.

Bit 5: H (Half Carry Flag)
This flag indicates if there was a carry from the lower 4 bits to the upper 4 bits
of a value during an operation.

Bit 4: C (Carry Flag)
This flag is used to indicate when an operation resulted in a carry or borrow.

Bits 3-0: Not used (Always Zero)
*/

void inst_to_str(cpu_context *ctx, char *str);