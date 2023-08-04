#include <stack.h>
#include <cpu.h>
#include <bus.h>


void stack_push(u8 data) { // Push 1byte onto the stack
    cpu_get_regs()->sp--;
    bus_write(cpu_get_regs()->sp, data);
    // Decrement the stack pointer and write data to the new address
}

void stack_push16(u16 data) { // Push 2bytes onto the stack
    stack_push((data >> 8) & 0xFF);
    stack_push(data & 0xFF);
}

u8 stack_pop() { // return the current byte on the stack and increment stack
    return bus_read(cpu_get_regs()->sp++);
}

u16 stack_pop16() {
u8 lo = stack_pop();
u8 hi = stack_pop();

return (hi << 8) | lo;
}