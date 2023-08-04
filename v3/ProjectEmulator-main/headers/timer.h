#pragma once

#include <common.h>

typedef struct { // Timer and Divider Registers
    u16 div; // Divider Register
    u8 tima; // Timer Counter
    u8 tma; // Timer Modulo
    u8 tac; // Timer Control.  Specifies clock frequency.
} timer_context;

void timer_init();
void timer_tick();

void timer_write(u16 address, u8 value);
u8 timer_read(u16 address);

timer_context *timer_get_context();