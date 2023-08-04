#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
// Shorthand usage of unsigned ints of 8-64 size.

#define BIT(a, n) ((a & (1 << n)) ? 1 : 0) 
// If the bit at n in a is 1, returns 1
// If the bit at n is 0, returns 0

#define BIT_SET(a, n, on) {if (on) a |= (1 << n); else a &= ~(1 << n);}
// If on is non-zero, then SET bit position n in a to 1.
// If on is zero, then CLEAR the bit at position n in a.

#define BETWEEN(a, b, c) ((a >= b) && (a <= c))
// If a is inclusively between b and c, return true.
// If a is not inclusively between b and c, return false.

void delay(u32 ms);

#define NO_IMPL { fprintf(stderr, "NOT YET IMPLEMENTED\n"); exit(-5); }