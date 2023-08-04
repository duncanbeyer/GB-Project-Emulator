#pragma once

#include <common.h>

typedef struct { // Object Attribute Memory
    // Objects can be 8x8 or 8x16, length extended downwards
    u8 y; // Y-position on the screen + 16
    u8 x; // X-position on the screen + 8
    u8 tile; // Tile index

    // Bits 0-3 of flags are used by CGB only. Bit4 palette #, bit5 flip over x,
    // bit6 flip over y, bit7 bg and window cover object

    u8 f_cgb_pn : 3;
    u8 f_cgb_vram_bank : 1;
    u8 f_pn : 1; // Palette #
    u8 f_x_flip : 1; // Flip over X axis (1 if mirrored)
    u8 f_y_flip : 1; // Flip over Y axis (1 if mirrored)
    u8 f_bgp : 1; // Cover under background and window layers

} oam_entry;


typedef struct {
    oam_entry oam_ram[40];
    u8 vram[0x2000];
} ppu_context;

void ppu_init();
void ppu_tick();

void ppu_oam_write(u16 address, u8 value);
u8 ppu_oam_read(u16 address);

void ppu_vram_write(u16 address, u8 value);
u8 ppu_vram_read(u16 address);