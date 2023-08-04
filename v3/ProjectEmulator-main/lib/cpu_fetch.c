#include <cpu.h>
#include <bus.h>
#include <emu.h>

extern cpu_context ctx; // ctx taken from cpu.c

// FILE *fp2;

void fetch_data() {
    ctx.memory_destination = 0;
    ctx.dest_is_mem = false;
    // These ^^^ two are reset at the beginning of each operation

    if (ctx.current_instruction == NULL) {
        return;
    }

    switch (ctx.current_instruction->mode) { // Checking each addressing mode

        // Addressing Operations are based partially on the Shart SM83 Instruction Set.

        case A_IMP: return; // Implied

        case A_R: // Single Register used
            ctx.fetch_data = cpu_read_reg(ctx.current_instruction->reg1);
            return;
        
        case A_R_R: // reading from a second register into reg1, retrieve data from reg2
            ctx.fetch_data = cpu_read_reg(ctx.current_instruction->reg2);
            return;

        case A_R_D8: // Reading an 8bit value and a register
            ctx.fetch_data = bus_read(ctx.regs.pc); // pc is already incremented
            emu_cycles(1);
            ctx.regs.pc++; // increment to point to the next instruction
            return;

        case A_R_D16: // same as A_D16
        case A_D16: { // Reading a 16bit value
            u16 lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);
            // Read the upper and lower 8 bits into two u16s, then combine in fetch_data
            ctx.fetch_data = lo | (hi << 8);

            ctx.regs.pc += 2; // increment the pc past the 16 bits used

            return;
        }

        case A_MR_R: // Reading from a register into memory
            ctx.fetch_data = cpu_read_reg(ctx.current_instruction->reg2); 
            // Fetch from reg2
            ctx.memory_destination = cpu_read_reg(ctx.current_instruction->reg1); 
            // Set memory_destination to the value at reg1
            ctx.dest_is_mem = true; // Trip because we are using a memory address

            if (ctx.current_instruction->reg1 == R_C) {
                ctx.memory_destination |= 0xFF00;
            } // When reading into the destination at register C, use OR operation
            // so that the range of C is changed from 0xFF-0x00 to 0xFFFF-0xFF00 as
            // specified in the hardware manual.
            return;

        case A_R_MR: {
            u16 addr = cpu_read_reg(ctx.current_instruction->reg2);           

            // Read memory address from reg2
            if (ctx.current_instruction->reg2 == R_C) {
                addr |= 0xFF00;
            } // Same as above case

            ctx.fetch_data = bus_read(addr); // Read the data from the address into fetch_data


            emu_cycles(1);
        } return;

        case A_R_HLI:
            ctx.fetch_data = bus_read(cpu_read_reg(ctx.current_instruction->reg2));
            emu_cycles(1); // Read data from HL register then increment the register
            cpu_set_reg(R_HL, cpu_read_reg(R_HL) + 1);
            return;

        case A_R_HLD:
            ctx.fetch_data = bus_read(cpu_read_reg(ctx.current_instruction->reg2));
            emu_cycles(1); // Read data from HL register then decrement the register
            cpu_set_reg(R_HL, cpu_read_reg(R_HL) - 1);
            return;

        case A_HLI_R:
            // Read data from a register then set memory_destination to the HL register
            // and trip the boolean. Then increment the HL register.
            ctx.fetch_data = cpu_read_reg(ctx.current_instruction->reg2);
            ctx.memory_destination = cpu_read_reg(ctx.current_instruction->reg1);
            ctx.dest_is_mem = true;
            cpu_set_reg(R_HL, cpu_read_reg(R_HL) + 1);
            return;

        case A_HLD_R:
            // Read data from a register then set memory_destination to the HL register
            // and trip the boolean. Then decrement the HL register.
            ctx.fetch_data = cpu_read_reg(ctx.current_instruction->reg2);
            ctx.memory_destination = cpu_read_reg(ctx.current_instruction->reg1);
            ctx.dest_is_mem = true;
            cpu_set_reg(R_HL, cpu_read_reg(R_HL) - 1);
            return;

        case A_R_A8:
            ctx.fetch_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return; // Read data from PC, which has already been incrememnted, into fetch

        case A_A8_R:
            // Set the memory destination to the location at PC
            ctx.memory_destination = bus_read(ctx.regs.pc) | 0xFF00;
            ctx.dest_is_mem = true;
            emu_cycles(1);
            ctx.regs.pc++;
            return;

        case A_HL_SPR:
            // Load the stack pointer into HL incremented by R8
            ctx.fetch_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return;

        case A_D8: 
            // Load the value at PC into fetch_data
            ctx.fetch_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return;

        case A_A16_R: // Same as A_D16_R
        case A_D16_R: {
            // Load a value from a register into fetch_data, load a 16bit address
            // into memory_destination and trip dest_is_mem
            u16 lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);
            // Read the upper and lower 8 bits into two u16s, then combine in fetch_data
            
            ctx.memory_destination = lo | (hi << 8);
            ctx.dest_is_mem = true;

            ctx.regs.pc += 2; // increment the pc past the 16 bits used
            ctx.fetch_data = cpu_read_reg(ctx.current_instruction->reg2);
        } return;

        case A_MR_D8:
            // Read the 8bit value at PC into fetch_data, load an for memory from a
            // register into memory_destination and trip dest_is_mem
            ctx.fetch_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            ctx.memory_destination = cpu_read_reg(ctx.current_instruction->reg1);
            ctx.dest_is_mem = true;
            return;

        case A_MR:
            // Load an address for memory from a register into memory_destination 
            // and trip dest_is_mem
            ctx.memory_destination = cpu_read_reg(ctx.current_instruction->reg1);
            ctx.dest_is_mem = true;
            ctx.fetch_data = bus_read(cpu_read_reg(ctx.current_instruction->reg1));
            emu_cycles(1);
            return;

        case A_R_A16: {
            u16 lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);
            // Read the upper and lower 8 bits into two u16s, then combine in fetch_data
            
            u16 addr = lo | (hi << 8);
            ctx.regs.pc += 2; // increment the pc past the 16 bits used

            ctx.fetch_data = bus_read(addr);
            emu_cycles(1);
            return;
        }

        default: 
            printf("Unknown Addressing Mode! %d\n", ctx.current_instruction->mode);
            exit(-7);
            return;
    }
}