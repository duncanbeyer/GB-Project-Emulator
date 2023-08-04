#include <bus.h>
#include <cart.h>
#include <ram.h>
#include <cpu.h>
#include <../headers/io.h>
#include <ppu.h>
#include <dma.h>

u8 bus_read(u16 address) {
    if (address < 0x8000) { 
        // Range of ROM Bank Data
        return cart_read(address);
    }
    else if (address < 0xA000) { 
        // Character / Background Map Data
        return ppu_vram_read(address);
    }
    else if (address < 0xC000) { 
        // Cartridge Ram
        return cart_read(address);
    }
    else if (address < 0xE000) { 
        // Working Ram
        return wram_read(address);
    }
    else if (address < 0xF000) { 
        // Reserved Echo Ram
        return 0;
    }
    else if (address < 0xFEA0) {
        // Object Attribute Memory
        if (dma_transferring()) {
            return 0xFF;
        }
        return ppu_oam_read(address);
    }
    else if (address < 0xFF00) {
        // reserved unusable
        return 0;
    }
    else if (address < 0xFF80) {
        return io_read(address);
    }
    else if (address == 0xFFFF) {
        return cpu_get_ie_reg();
    }
    return hram_read(address);
}

u16 bus_read16(u16 address) {
    u16 lo = bus_read(address);
    u16 hi = bus_read(address + 1);

    return lo | (hi << 8);
}

void bus_write(u16 address, u8 value) {
    if (address < 0x8000) {
        // Range of ROM Bank Data
        cart_write(address, value);
    }
    else if (address < 0xA000) { 
        // Character / Background Map Data
        ppu_vram_write(address, value);
    }
    else if (address < 0xC000) { 
        // Cartridge Ram
        cart_write(address, value);
    }
    else if (address < 0xE000) { 
        // Working Ram
        wram_write(address, value);
    }
    else if (address < 0xFE00) { 
        // Reserved Echo Ram
        //NO_IMPL;
    }
    else if (address < 0xFEA0) {
        // Object Attribute Ram
        if (dma_transferring()) {
            return;
        }
        ppu_oam_write(address, value);
    }
    else if (address < 0xFF00) {
        // Unusable Reserved
    }
    else if (address < 0xFF80) {
        // IO Registers
        io_write(address, value);
    }
    else if (address == 0xFFFF) {
        // Object Attribute Ram
        cpu_set_ie_reg(value);
    }
    else {
        hram_write(address,value);
    }
}

void bus_write16(u16 address, u16 value) {
    
    bus_write(address + 1, (value >> 8) & 0xFF); // Write the high byte 
    bus_write(address, 0xFF & value); // Write the low byte
}