#pragma once

#include <common.h>

static const int LINES_PER_FRAME = 154;
static const int TICKS_PER_LINE = 456;
static const int YRES = 144;
static const int XRES = 160;

typedef enum {
    FS_TILE,
    FS_DATA0,
    FS_DATA1,
    FS_IDLE,
    FS_PUSH
} fetch_state;

typedef struct _fifo_entry {
    struct _fifo_entry *next;
    u32 value; // 32 bit color value
} fifo_entry;


typedef struct {
    fifo_entry *head;
    fifo_entry *tail;
    u32 size;
} fifo;

typedef struct {
    fetch_state cur_fetch_state;
    fifo pixel_fifo;
    u8 line_x;
    u8 pushed_x;
    u8 fetch_x;
    u8 bgw_fetch_data[3];
    u8 fetch_entry_data[6];
    u8 map_y;
    u8 map_x;
    u8 tile_y;
    u8 fifo_x;
} pixel_fifo_context;

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

    pixel_fifo_context pfc;

    u32 current_frame; // current frame we are on
    u32 line_ticks; // how many ticks there are on the line currently
    u32 *video_buffer;
} ppu_context;

ppu_context *ppu_get_context();

void ppu_init();
void ppu_tick();

void ppu_oam_write(u16 address, u8 value);
u8 ppu_oam_read(u16 address);

void ppu_vram_write(u16 address, u8 value);
u8 ppu_vram_read(u16 address);