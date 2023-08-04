#include <cpu.h>
#include <bus.h>
#include <emu.h>
#include <interrupts.h>
#include <dbg.h>
#include <timer.h>

cpu_context ctx = {0};
bool debugger = false;
FILE *fp;
// FILE *fp2;

void cpu_init() { // initialize default cpu register values
    ctx.regs.pc = 0x100;
    ctx.regs.sp = 0xFFFE;
    *((short *)&ctx.regs.a) = 0xB001;
    *((short *)&ctx.regs.b) = 0x1300;
    *((short *)&ctx.regs.d) = 0xD800;
    *((short *)&ctx.regs.h) = 0x4D01;
    ctx.ie_register = 0;
    ctx.int_flags = 0;
    ctx.int_master_enabled = false;
    ctx.enabling_ime = false;
    fp = fopen("output.txt", "w+");
    // fp2 = fopen("hram.txt", "w+");


    timer_get_context()->div = 0xABCC;
}

void fetch_instruction() { // read the opcode, turn it into an instruction
    ctx.current_opcode = bus_read(ctx.regs.pc++); // increment the program count
    ctx.current_instruction = opcode_to_instruction(ctx.current_opcode);
}

void fetch_data();

static void execute() {
    IN_PROC proc = inst_get_processor(ctx.current_instruction->type);

    if (!proc) {
        NO_IMPL
    }

    proc(&ctx);
}

bool cpu_step() {
    if (!ctx.paused) {

        u16 pc = ctx.regs.pc;

        fetch_instruction();
        emu_cycles(1);
        fetch_data();

        char flags[16];
        sprintf(flags, "%c%c%c%c", 
            ctx.regs.f & (1 << 7) ? 'Z' : '-',
            ctx.regs.f & (1 << 6) ? 'N' : '-',
            ctx.regs.f & (1 << 5) ? 'H' : '-',
            ctx.regs.f & (1 << 4) ? 'C' : '-'
        );

        char inst[16];
        inst_to_str(&ctx, inst);

        // printf("%08lX - %04X: %-12s (%02X %02X %02X) A: %02X F: %s BC: %02X%02X DE: %02X%02X HL: %02X%02X\n", 
        //     emu_get_context()->ticks,
        //     pc, inst, ctx.current_opcode,
        //     bus_read(pc + 1), bus_read(pc + 2), ctx.regs.a, flags, ctx.regs.b, ctx.regs.c,
        //     ctx.regs.d, ctx.regs.e, ctx.regs.h, ctx.regs.l);

        fprintf(fp, "%08lX - %04X: %-12s (%02X %02X %02X) A: %02X F: %s BC: %02X%02X DE: %02X%02X HL: %02X%02X\n", 
            emu_get_context()->ticks,
            pc, inst, ctx.current_opcode,
            bus_read(pc + 1), bus_read(pc + 2), ctx.regs.a, flags, ctx.regs.b, ctx.regs.c,
            ctx.regs.d, ctx.regs.e, ctx.regs.h, ctx.regs.l);

        if (ctx.current_instruction == NULL) {
            printf("Unknown Instruction! %02X\n", ctx.current_opcode);
            exit(-7);
        }

        if (emu_get_context()->ticks >= 0xA50000) {
            fclose(fp);
            exit(-7);
        }
        dbg_update();
        dbg_print();
        execute();
    }
    else {
        printf("cpu is halted\n");
        // is halted ...
        emu_cycles(1);
        if (ctx.int_flags) {
            ctx.paused = false;
        }

    }
    if (ctx.int_master_enabled) {
        cpu_handle_interrupts(&ctx);
        ctx.enabling_ime = false;
    }
    if (ctx.enabling_ime) {
        ctx.int_master_enabled = true;
    }
    return true;
}

u8 cpu_get_ie_reg() {
    return ctx.ie_register;
}
void cpu_set_ie_reg(u8 value) {
    ctx.ie_register = value;
}

void cpu_request_interrupt(interrupt_type t) {
    ctx.int_flags |= t;
}