#include <ram.h>
#include <cart.h>
#include <emu.h>
// #include <cpu.h>

typedef struct {
    u8 wram[0x2000]; // working ram
    u8 hram[0x80]; // high ram
} ram_context;

// FILE *fp2;

static ram_context ctx;

u8 wram_read(u16 address) {

    address -= 0xC000; 
    // wram is a 0x0000 - 0x2000 range starting at 0xC000

    if (address >= 0x2000) {
        // printf("INVALID WRAM ADDR %08X\n", address + 0xC000);
        exit(-1);
    }

    return ctx.wram[address];
}


void wram_write(u16 address, u8 value) {
    address -= 0xC000; 

    ctx.wram[address] = value;
}

u8 hram_read(u16 address) {
    // fprintf(fp2, "reading from hram addr %04X value %02X\n", address, ctx.hram[address]);

    address -= 0xFF80; 
    // wram is a 0x00 - 0x80 range starting at 0xFF80
    // fp2 = fopen("hram.txt", "a");
    // fprintf(fp2, "reading hram addr %02X value is %02X\n", address, ctx.hram[address]);
    // fprintf(fp2, "ticks is %09lX\n\n", emu_get_context()->ticks);
    // fclose(fp2);
    // if (ctx.hram[address] == 0x03) {
    //     printf("reading hram 0xFF83 the val is %02X\n", ctx.hram[address]);
    // }

    return ctx.hram[address];
}

void hram_write(u16 address, u8 value) {
    address -= 0xFF80; 
    // fp2 = fopen("hram.txt", "a");
    // fprintf(fp2, "writing to hram addr %02X value %02X\n", address, value);
    // fprintf(fp2, "ticks is %09lX\n", emu_get_context()->ticks);
    // fclose(fp2);
    ctx.hram[address] = value;
}