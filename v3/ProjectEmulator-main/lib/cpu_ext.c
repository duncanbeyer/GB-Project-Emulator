#include <cpu.h>
#include <bus.h>

extern cpu_context ctx;

u16 reverse(u16 n) {
    return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}

u16 cpu_read_reg(register_type rt) { // Reading registers based on ENUMs.
    switch(rt) {
        case R_A: return ctx.regs.a;
        case R_F: return ctx.regs.f;
        case R_B: return ctx.regs.b;
        case R_C: return ctx.regs.c;
        case R_D: return ctx.regs.d;
        case R_E: return ctx.regs.e;
        case R_H: return ctx.regs.h;
        case R_L: return ctx.regs.l;

        case R_AF: return reverse(*((u16 *)&ctx.regs.a));
        case R_BC: return reverse(*((u16 *)&ctx.regs.b));
        case R_DE: return reverse(*((u16 *)&ctx.regs.d));
        case R_HL: return reverse(*((u16 *)&ctx.regs.h));

        case R_PC: return ctx.regs.pc;
        case R_SP: return ctx.regs.sp;
        default: return 0;
    }
}


void cpu_set_reg(register_type rt, u16 val) { // Setting registers based on ENUMs.
    switch(rt) {
        case R_A: ctx.regs.a = val & 0xFF; break;
        case R_F: ctx.regs.f = val & 0xFF; break;
        case R_B: ctx.regs.b = val & 0xFF; break;
        case R_C: {
            ctx.regs.c = val & 0xFF; 
        } break;
        case R_D: ctx.regs.d = val & 0xFF; break;
        case R_E: ctx.regs.e = val & 0xFF; break;
        case R_H: ctx.regs.h = val & 0xFF; break;
        case R_L: ctx.regs.l = val & 0xFF; break;

        case R_AF: *((u16 *)&ctx.regs.a) = reverse(val); break;
        case R_BC: *((u16 *)&ctx.regs.b) = reverse(val); break;
        case R_DE: *((u16 *)&ctx.regs.d) = reverse(val); break;
        case R_HL: *((u16 *)&ctx.regs.h) = reverse(val); break;

        case R_PC: ctx.regs.pc = val; break;
        case R_SP: ctx.regs.sp = val; break;
        case R_NONE: break;
    }
}


u8 cpu_read_reg8(register_type rt) {
    switch(rt) {
        case R_A: return ctx.regs.a;
        case R_F: return ctx.regs.f;
        case R_B: return ctx.regs.b;
        case R_C: return ctx.regs.c;
        case R_D: return ctx.regs.d;
        case R_E: return ctx.regs.e;
        case R_H: return ctx.regs.h;
        case R_L: return ctx.regs.l;

        case R_HL: {
            return bus_read(cpu_read_reg(R_HL));
        }

        default: 
            printf("ERROR INVALID REG8\n");
            NO_IMPL
    } 
}

void cpu_set_reg8(register_type rt, u8 val) {
    switch(rt) {
        case R_A: ctx.regs.a = val & 0xFF; break;
        case R_F: ctx.regs.f = val & 0xFF; break;
        case R_B: ctx.regs.b = val & 0xFF; break;
        case R_C: ctx.regs.c = val & 0xFF; break;
        case R_D: ctx.regs.d = val & 0xFF; break;
        case R_E: ctx.regs.e = val & 0xFF; break;
        case R_H: ctx.regs.h = val & 0xFF; break;
        case R_L: ctx.regs.l = val & 0xFF; break;

        case R_HL: {
            bus_write(cpu_read_reg(R_HL), val); break;
        }

        default: 
            printf("ERROR INVALID REG8\n");
            NO_IMPL
    } 
}



cpu_registers *cpu_get_regs() {
    return &ctx.regs;
}

u8 cpu_get_int_flags() {
    return ctx.int_flags;
}

void cpu_set_int_flags(u8 val) {
    ctx.int_flags = val;
}