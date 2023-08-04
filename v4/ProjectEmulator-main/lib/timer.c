#include <timer.h>
#include <interrupts.h>

static timer_context ctx = {0};

timer_context *timer_get_context() {
    return &ctx;
}

void timer_init() {
    ctx.div = 0xAC00; // Starting value for divider
}

void timer_tick() {
    u16 prev_div = ctx.div;
    ctx.div++;

    bool timer_update = false;

    switch(ctx.tac & (0b11)) {
        // Lowest 2 bits of the tac determine which bit of div to check to determine if timer gets updated
        // If the bit is changed after ctx.div is incremented above, then need to update the timer.
        case 0b00:
            // Checking 9th bit
            timer_update = (prev_div & (1 << 9)) && (!(ctx.div & (1 << 9)));
            break;
        case 0b01:
            // Checking 3rd bit
            timer_update = (prev_div & (1 << 3)) && (!(ctx.div & (1 << 3)));
            break;
        case 0b10:
            // checking 5th
            timer_update = (prev_div & (1 << 5)) && (!(ctx.div & (1 << 5)));
            break;
        case 0b11:
            // Checking 7th
            timer_update = (prev_div & (1 << 7)) && (!(ctx.div & (1 << 7)));
            break;
    }


    if (timer_update && ctx.tac & (1<<2)) {
        // The third bit must also be enabled to update the timer
        ctx.tima++;

        if (ctx.tima == 0xFF) { // If the tima is maxxed out,
            ctx.tima = ctx.tma; // then reset it to equal tma and request interrupt.

            cpu_request_interrupt(IT_TIMER);
        }
    }
}

void timer_write(u16 address, u8 value) {
    switch(address) {
        case 0xFF04: // Whenever writing to FF04, set div = 0.
            ctx.div = 0;
            break;
        case 0xFF05:
            ctx.tima = value;
            break;
        case 0xFF06:
            ctx.tma = value;
            break;
        case 0xFF07:
            ctx.tac = value;
            break;

    }
}

u8 timer_read(u16 address) {
    switch(address) {
        case 0xFF04: // Only care about the top byte for this register
            return ctx.div >> 8; 
        case 0xFF05:
            return ctx.tima;
        case 0xFF06:
            return ctx.tma;
        case 0xFF07:
            return ctx.tac;
    }
}