#include <ppu.h>

static ppu_context ctx;

void ppu_init() {

}
void ppu_tick() {

}

void ppu_oam_write(u16 address, u8 value) {
    if (address >= 0xFE00) {
        // Remove the memory address offset to just get the value
        address -= 0xFE00;
    }

    u8 *p = (u8 *)ctx.oam_ram;
    // Access oam_ram as unsigned 8bit int pointer
    p[address] = value;
    // Write the value to the pointer at address
}

u8 ppu_oam_read(u16 address) {
    if (address >= 0xFE00) {
        // Remove the memory address offset to just get the value
        address -= 0xFE00;
    }

    u8 *p = (u8 *)ctx.oam_ram;
    // Access oam_ram as unsigned 8bit int pointer
    return p[address];
    // Read the value from the pointer at address
}

void ppu_vram_write(u16 address, u8 value) {
    ctx.vram[address - 0x8000] = value;
}
u8 ppu_vram_read(u16 address) {
    return ctx.vram[address - 0x8000];
}