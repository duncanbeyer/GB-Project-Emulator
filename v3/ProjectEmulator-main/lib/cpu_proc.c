#include <cpu.h>
#include <emu.h>
#include <bus.h>
#include <stack.h>
#include <instructions.h>
#include <stdint.h>

// FILE *fp2;

void cpu_set_flags(cpu_context *ctx, int8_t z, int8_t n, int8_t h, int8_t c) {
    if (z != -1) {
        BIT_SET(ctx->regs.f, 7, z);
    }
    if (n != -1) {
        BIT_SET(ctx->regs.f, 6, n);
    }
    if (h != -1) {
        BIT_SET(ctx->regs.f, 5, h);
    }
    if (c != -1) {
        BIT_SET(ctx->regs.f, 4, c);
    }
}

static void proc_none(cpu_context *ctx) {
    printf("INVALID INSTRUCTION!\n");
    exit(-7);
}

static void proc_nop(cpu_context *ctx) {
}

register_type rt_lookup[] = { // Registers for CB ops
    R_B,
    R_C,
    R_D,
    R_E,
    R_H,
    R_L,
    R_HL,
    R_A
};

register_type decode_reg(u8 reg) {

    if (reg > 0b111) {
        return R_NONE;
    }

    return rt_lookup[reg];

}

static void proc_cb(cpu_context *ctx) {
    u8 op = ctx->fetch_data;
    register_type reg = decode_reg(op & 0b111);
    // Only the bottom 3 bits are needed to decode which register to use for CB
    u8 bit = (op >> 3) & 0b111;
    u8 bit_op = (op >> 6) & 0b11;
    u8 reg_val = cpu_read_reg8(reg);

    emu_cycles(1);

    if (reg == R_HL) {
        emu_cycles(2);
    }

    switch(bit_op) {
        case 1:
            // BIT operation: checking if a bit on a register is set by setting zero flag if it is zero
            cpu_set_flags(ctx, !(reg_val & (1 << bit)), 0, 1, -1);
            return;
        case 2:
            // RST operation: flips the specified bit
            reg_val &= ~(1 << bit);
            cpu_set_reg8(reg, reg_val);
            return;
        case 3:
            // SET operation: sets the specified bit
            reg_val |= (1 << bit);
            cpu_set_reg8(reg, reg_val);
            return;
    }

    bool c = CPU_FLAG_C;

    switch (bit) {
        case 0: {
            // RLC: Rotate left old bit 7 to the carry flag
            bool setC = false;
            u8 result = (reg_val << 1) & 0xFF;
            // Move all bits in the register 1bit to the left

            if ((reg_val & (1 << 7)) != 0) {
                // If the register value's msb was set, then a carry happened
                result |= 1;
                setC = true;
            }

            cpu_set_reg8(reg, result);
            cpu_set_flags(ctx, result == 0, false, false, setC);
        } return;
        case 1: {
            // RRC: Rotate right old bit 0 to the carry flag
            u8 old = reg_val;
            reg_val >>= 1;
            reg_val |= (old << 7);

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(ctx, !reg_val, false, false, old & 1);
        } return;
        case 2: {
            // RL: Rotate the register value left

            u8 old = reg_val;
            reg_val <<= 1;
            reg_val |= c;

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(ctx, !reg_val, false, false, !!(old & 0x80));
            //                                              ^^^^^^^^^
            // Carry flag is true if bit 7 of the old register was set aka carry happened
        } return;
        case 3: {
            // RR: Rotate the register value right
            u8 old = reg_val;
            reg_val >>= 1;
            reg_val |= (c << 7);

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(ctx, !reg_val, false, false, old & 1);
        } return;
        case 4: {
            // SLA: Shift left and carry
            u8 old = reg_val;
            reg_val <<= 1;

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(ctx, !reg_val, false, false, !!(old & 0x80));
        } return;
        case 5: {
            // SRA: Shift right and carry
            u8 u = (int8_t)reg_val >> 1;

            cpu_set_reg8(reg, u);
            cpu_set_flags(ctx, !u, 0, 0, reg_val & 1);

        } return;
        case 6: {
            // SWAP: Swapping the high and low nibble
            reg_val = ((reg_val & 0xF0) >> 4) | ((reg_val & 0xF) << 4);
            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(ctx, reg_val == 0, false, false, false);
        } return;
        case 7: {
            // SRL: Shift right into carry
            u8 u = reg_val >> 1;
            cpu_set_reg8(reg, u);
            cpu_set_flags(ctx, !u, 0, 0, reg_val & 1);
        } return;
    }

    printf("Error: Invalid CB: %02X\n", op);
    NO_IMPL
}

static void proc_rlca(cpu_context *ctx) {
    // Rotate left with carry
    u8 u = ctx->regs.a;
    bool c = (u >> 7) & 1;
    u = (u << 1) | c;
    ctx->regs.a = u;

    cpu_set_flags(ctx, 0, 0, 0, c);
}

static void proc_rrca(cpu_context *ctx) {
    // Rotate right with carry
    u8 b = ctx->regs.a & 1;
    ctx->regs.a >>= 1;
    ctx->regs.a |= (b << 7);

    cpu_set_flags(ctx, 0, 0, 0, b);
}

static void proc_rla(cpu_context *ctx) {
    // Rotate left
    u8 u = ctx->regs.a;
    u8 cf = CPU_FLAG_C;
    u8 c = (u >> 7) & 1;
    ctx->regs.a = (u << 1) | cf;

    cpu_set_flags(ctx, 0, 0, 0, c);
}

static void proc_rra(cpu_context *ctx) {
    // Rotate right
    u8 c = CPU_FLAG_C;
    u8 newC = ctx->regs.a & 1;

    ctx->regs.a >>= 1;
    ctx->regs.a |= (c << 7);

    cpu_set_flags(ctx, 0, 0, 0, newC);

}

static void proc_daa(cpu_context *ctx) {
    // Decimal Adjust Accumulator
    // The only instruction that uses the N and H flags
    u8 u = 0;
    int fc = 0;

    if (CPU_FLAG_H || (!CPU_FLAG_N && (ctx->regs.a & 0xF) > 9)) {
        u = 6;
    }

    if (CPU_FLAG_C || (!CPU_FLAG_N && ctx->regs.a > 0x99)) {
        u |= 0x60;
        fc = 1;
    }

    ctx->regs.a += CPU_FLAG_N ? -u : u;

    // if (CPU_FLAG_N) {
    //     ctx->regs.a -= u;
    // }
    // else {
    //     ctx->regs.a += u;
    // }

    cpu_set_flags(ctx, ctx->regs.a == 0, -1, 0, fc);
}

static void proc_cpl(cpu_context *ctx) {
    // Flipping the Accumulator
    ctx->regs.a = ~ctx->regs.a;
    cpu_set_flags(ctx, -1, 1, 1, -1);
}

static void proc_scf(cpu_context *ctx) {
    cpu_set_flags(ctx, -1, 0, 0, 1);
}

static void proc_ccf(cpu_context *ctx) {
    cpu_set_flags(ctx, -1, 0, 0, CPU_FLAG_C ^ 1);
}

static void proc_halt(cpu_context *ctx) {
    ctx->paused = true;
}

static void proc_and(cpu_context *ctx) {
    // Bitwise and between a val and A, store in A.

    ctx->regs.a &= ctx->fetch_data;
    cpu_set_flags(ctx, ctx->regs.a == 0, 0, 1, 0);

}

static void proc_or(cpu_context *ctx) {
    ctx->regs.a |= ctx->fetch_data & 0xFF;
    cpu_set_flags(ctx, ctx->regs.a == 0, 0, 0, 0);
}

static void proc_cp(cpu_context *ctx) {
    // Compare; similar to subtract but A does not change, only flags.
    int val = (int)ctx->regs.a - (int)ctx->fetch_data;
    int z = val == 0;
    int h = ((int)ctx->regs.a & 0x0F) - ((int)ctx->fetch_data & 0x0F) < 0;
    // If a the first nibble caused a carry
    int c = val < 0;
    // If the byte caused a carry

    cpu_set_flags(ctx, z, 1, h, c);

}   

static void proc_di(cpu_context *ctx) { 
    // Disable interrupts
    ctx->int_master_enabled = false;
}

static void proc_ei(cpu_context *ctx) { 
    // Disable interrupts
    ctx->enabling_ime = true;
}

static bool is_16(register_type rt) {
    // Check if the register type is greater than 16bits
    return rt >= R_AF;
}

static void proc_ld(cpu_context *ctx) {
    if (ctx->dest_is_mem) { 
        // in case LD (A), B  or LD (a16), AF where dest is an address
        if (is_16(ctx->current_instruction->reg2)) {
            // If the data is 16bit
            emu_cycles(1);
            bus_write16(ctx->memory_destination, ctx->fetch_data);
        } 
        else {
        bus_write(ctx->memory_destination, ctx->fetch_data);
        }
 
        emu_cycles(1);

        return;
    }
    if (ctx->current_instruction->mode == A_HL_SPR) {
        u8 hflag = (cpu_read_reg(ctx->current_instruction->reg2) & 0xF) + 
            (ctx->fetch_data & 0xF) >= 0x10; // Check for half carry
        u8 cflag = (cpu_read_reg(ctx->current_instruction->reg2) & 0xFF) + 
            (ctx->fetch_data & 0xFF) >= 0x100; // True if result goes over 8bits

        cpu_set_flags(ctx, 0, 0, hflag, cflag);

        cpu_set_reg(ctx->current_instruction->reg1, 
        cpu_read_reg(ctx->current_instruction->reg2) + (char)ctx->fetch_data);

        return;

    }
    
    // fp2 = fopen("hram.txt", "a");
    // fprintf(fp2, "about to set reg %d to fetch %02X from fetch in ld\n", ctx->current_instruction->reg1, ctx->fetch_data);
    // fprintf(fp2, "ticks is now %09lX\n", emu_get_context()->ticks);
    // fclose(fp2);

    // if the destination is a CPU register
    cpu_set_reg(ctx->current_instruction->reg1, ctx->fetch_data);
}



static void proc_xor(cpu_context *ctx) {
    
    ctx->regs.a ^= ctx->fetch_data & 0xFF;
    cpu_set_flags(ctx, ctx->regs.a == 0, 0, 0, 0);
}

static void proc_ldh(cpu_context *ctx) {
    // Load into hram
    if (ctx->current_instruction->reg1 == R_A) { // If loading into Accumulator
        cpu_set_reg(ctx->current_instruction->reg1, bus_read(0xFF00 | ctx->fetch_data));
        // Fetch data is ORed with 0xFF00 because that is how hram is accessed from 
        // bus_read.
    }
    else { // If loading FROM Accumulator
        bus_write(ctx->memory_destination, ctx->regs.a);
    }
    emu_cycles(1);
}



static bool check_cond(cpu_context *ctx) {

    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch(ctx->current_instruction->condition) {
        case C_NONE: return true;
        case C_C: return c;
        case C_NC: return !c;
        case C_Z: return z;
        case C_NZ: return !z;
    }

    return false;
}

static void goto_addr(cpu_context *ctx, u16 addr, bool pushpc) {
    if (check_cond(ctx)) {
        if (pushpc) {
            emu_cycles(2);
            stack_push16(ctx->regs.pc);
        }
        ctx->regs.pc = addr;
        emu_cycles(1);
    }
}

static void proc_jp(cpu_context *ctx) {
    // Jump to address in fetch_data, don't push the stack
    goto_addr(ctx, ctx->fetch_data, false);
}

static void proc_jr(cpu_context *ctx) {
    int8_t rel = (int8_t)(ctx->fetch_data & 0xFF);
    // Changed from ^^^ char to int8_t
    u16 addr = ctx->regs.pc + rel;
    goto_addr(ctx, addr, false);
}


 // GOT TO HERE BUGFIXING 2:51 PM


 
static void proc_pop(cpu_context *ctx) {
    // Pop the stack into a 2byte cpu register
    u16 lo = stack_pop();
    emu_cycles(1);
    u16 hi = stack_pop();
    emu_cycles(1);
    u16 ans = (hi << 8) | lo;

    cpu_set_reg(ctx->current_instruction->reg1, ans);

    if (ctx->current_instruction->reg1 == R_AF) {
        cpu_set_reg(ctx->current_instruction->reg1, ans & 0xFFF0);
    }
}

static void proc_push(cpu_context *ctx) {
    u16 hi = (cpu_read_reg(ctx->current_instruction->reg1) >> 8) & 0xFF;
    emu_cycles(1);
    stack_push(hi); // Push the high byte of the stack pointer address

    u16 lo = (cpu_read_reg(ctx->current_instruction->reg1)) & 0xFF;
    emu_cycles(1);
    stack_push(lo); // Push the low byte of the stack pointer address

    emu_cycles(1);
}

static void proc_call(cpu_context *ctx) {
    goto_addr(ctx, ctx->fetch_data, true);
}

static void proc_rst(cpu_context *ctx) {
    goto_addr(ctx, ctx->current_instruction->param, true);
}

static void proc_ret(cpu_context *ctx) {
    if (ctx->current_instruction->condition != C_NONE) {
        // Extra cycle for checking a condition. If there is 
        // a condition it takes another cycle to check.
        emu_cycles(1);
    }

    if (check_cond(ctx)) {

        u16 lo = stack_pop();
        emu_cycles(1);

        u16 hi = stack_pop();
        emu_cycles(1);

        u16 t = (hi << 8) | lo;
        ctx->regs.pc = t;

        emu_cycles(1);
    }
}

static void proc_reti(cpu_context *ctx) {
    // Return from interrupt: uninterrupt, then return.
    ctx->int_master_enabled = true;
    proc_ret(ctx);
}



static void proc_dec(cpu_context *ctx) {
    u16 val = cpu_read_reg(ctx->current_instruction->reg1) - 1;
    if (is_16(ctx->current_instruction->reg1)) {
        emu_cycles(1);
    }

    if (ctx->current_instruction->reg1 == R_HL && ctx->current_instruction->mode == A_MR) {
        // This is the only DEC case that uses MR. Read the address
        // from HL and write back into the addr
        val = bus_read(cpu_read_reg(R_HL)) - 1;
        bus_write(cpu_read_reg(R_HL), val);
    }
    else {
        cpu_set_reg(ctx->current_instruction->reg1, val);
        val = cpu_read_reg(ctx->current_instruction->reg1);
    }

    if ((ctx->current_opcode & 0xB) == 0xB) {
        // INC ops 0x0B, 0x1B, 0x2B, and 0x3B do not set flags
        return;
    }

    cpu_set_flags(ctx, val == 0, 1, (val & 0x0F) == 0x0F, -1);
    // Z = 1 if val is 0, N = 1 because using subtraction, H is 1
    // if half carry was used, C is unchanged by -1.
}

static void proc_inc(cpu_context *ctx) {
    u16 val = cpu_read_reg(ctx->current_instruction->reg1) + 1;
    if (is_16(ctx->current_instruction->reg1)) {
        emu_cycles(1);
    }

    if (ctx->current_instruction->reg1 == R_HL && ctx->current_instruction->mode == A_MR) {
        // This is the only INC case that uses MR. Read the address
        // from HL, & to keep only 1byte, and write back into the addr
        val = (bus_read(cpu_read_reg(R_HL)) + 1) & 0xFF;
        bus_write(cpu_read_reg(R_HL), val);
    }
    else {
        cpu_set_reg(ctx->current_instruction->reg1, val);
        val = cpu_read_reg(ctx->current_instruction->reg1);
    }

    if ((ctx->current_opcode & 0x3) == 0x3) {
        // INC ops 0x03, 0x13, 0x23, and 0x33 do not set flags
        return;
    }

    cpu_set_flags(ctx, val == 0, 0, (val & 0x0F) == 0, -1);
    // Z = 1 if val is 0, N = 0 because using addition, H is 1
    // if half carry was used, C is unchanged by -1.
}

static void proc_add(cpu_context *ctx) {

    u32 val = cpu_read_reg(ctx->current_instruction->reg1) + ctx->fetch_data;
    bool is_16bit = is_16(ctx->current_instruction->reg1);

    if (is_16bit) {
        emu_cycles(1);
    }

    if (ctx->current_instruction->reg1 == R_SP) {
        // Special case opcode 0xE8
        val = cpu_read_reg(ctx->current_instruction->reg1) + (char)ctx->fetch_data;
    }

    int z = (val & 0xFF) == 0; 
    // z = 1 if result is 0.
    int h = (cpu_read_reg(ctx->current_instruction->reg1) & 0xF) + (ctx->fetch_data & 0xF) >= 0x10;
    // h = 1 if there was a half carry.
    int c = (int)(cpu_read_reg(ctx->current_instruction->reg1) & 0xFF) + (int)(ctx->fetch_data & 0xFF) >= 0x100;
    // c = 1 if there was a carry past 1byte

    if (is_16bit) {
        // When adding two 16bit vals, op flags are different.
        z = -1;
        h = (cpu_read_reg(ctx->current_instruction->reg1) & 0xFFF) + (ctx->fetch_data & 0xFFF) >= 0x1000;
        // If there was a half carry between two 16bit vals
        u32 n = ((u32)cpu_read_reg(ctx->current_instruction->reg1) + ((u32)ctx->fetch_data));
        c = n >= 0x10000;
        // c = 1 if there was a carry past 0xFFFF
    }

    if (ctx->current_instruction->reg1 == R_SP) {
        // opcode 0xE8 has its own flags
        z = 0;
        h = (cpu_read_reg(ctx->current_instruction->reg1) & 0xF) + (ctx->fetch_data & 0xF) >= 0x10;
        c = (int)(cpu_read_reg(ctx->current_instruction->reg1) & 0xFF) + (int)(ctx->fetch_data & 0xFF) >= 0x100;
    }
    cpu_set_reg(ctx->current_instruction->reg1, val & 0xFFFF);
    cpu_set_flags(ctx, z, 0, h, c);
    // The n flag is always set to 0 because the op is addition
}

static void proc_adc(cpu_context *ctx) {
    // Add with carry.

    u16 u = ctx->fetch_data;
    u16 a = ctx->regs.a;
    u16 c = CPU_FLAG_C;

    ctx->regs.a = (a + u + c) & 0xFF;
    cpu_set_flags(ctx, ctx->regs.a == 0, 0, (a & 0xF) + (u & 0xF) + c > 0xF, a + u + c > 0xFF);
}

static void proc_sub(cpu_context *ctx) {
    u16 val = cpu_read_reg(ctx->current_instruction->reg1) - ctx->fetch_data;

    int z = val == 0;
    int h = ((int)cpu_read_reg(ctx->current_instruction->reg1) & 0xF) - ((int)ctx->fetch_data & 0xF) < 0;
    // If a the first nibble caused a carry
    int c = ((int)cpu_read_reg(ctx->current_instruction->reg1)) - ((int)ctx->fetch_data) < 0;
    // If the byte caused a carry

    cpu_set_reg(ctx->current_instruction->reg1, val);
    cpu_set_flags(ctx, z, 1, h, c);

}

static void proc_sbc(cpu_context *ctx) {
    // Subtract with carry.
    u8 val = ctx->fetch_data + CPU_FLAG_C;

    int z = (cpu_read_reg(ctx->current_instruction->reg1) - val) == 0;
    int h = ((int)cpu_read_reg(ctx->current_instruction->reg1) & 0xF) - ((int)CPU_FLAG_C) - ((int)ctx->fetch_data & 0xF) < 0;
    // If a the first nibble caused a carry
    int c = (int)cpu_read_reg(ctx->current_instruction->reg1) - ((int)CPU_FLAG_C) - (int)ctx->fetch_data < 0;
    // If the byte caused a carry

    cpu_set_reg(ctx->current_instruction->reg1, cpu_read_reg(ctx->current_instruction->reg1) - val);
    cpu_set_flags(ctx, z, 1, h, c);

}

static void proc_stop(cpu_context *ctx) {
    printf("Stopping ...\n");
    NO_IMPL
}

static IN_PROC processors[] = { // Indices of Processes used by CPU Operations
    [I_NONE] = proc_none,
    [I_NOP] = proc_nop,
    [I_LD] = proc_ld,
    [I_JP] = proc_jp,
    [I_DI] = proc_di,
    [I_XOR] = proc_xor,
    [I_LDH] = proc_ldh,
    [I_POP] = proc_pop,
    [I_PUSH] = proc_push,
    [I_CALL] = proc_call,
    [I_JR] = proc_jr,
    [I_RET] = proc_ret,
    [I_RETI] = proc_reti,
    [I_RST] = proc_rst,
    [I_DEC] = proc_dec,
    [I_INC] = proc_inc,
    [I_ADD] = proc_add,
    [I_ADC] = proc_adc,
    [I_SUB] = proc_sub,
    [I_SBC] = proc_sbc,
    [I_AND] = proc_and,
    [I_OR] = proc_or,
    [I_CP] = proc_cp,
    [I_CB] = proc_cb,
    [I_RLCA] = proc_rlca,
    [I_RRCA] = proc_rrca,
    [I_RLA] = proc_rla,
    [I_RRA] = proc_rra,
    [I_STOP] = proc_stop,
    [I_HALT] = proc_halt,
    [I_DAA] = proc_daa,
    [I_CPL] = proc_cpl,
    [I_SCF] = proc_scf,
    [I_CCF] = proc_ccf,
    [I_EI] = proc_ei

};

IN_PROC inst_get_processor(instruction_type x) { // Get Processor needed by Opcode
    return processors[x];
}