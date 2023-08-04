#pragma once

#include <common.h>

typedef struct {
    u8 entry[4]; // 0x0100 - 0x0103 - Entry Point
    u8 logo[0x30]; // 0x0103 - 0x0133 - Nintendo Logo

    char title[16]; // 0x0134 - 0x0143 - Title
    // Title of the game is in upper case ASCII
    u16 new_lic_code; // 0x0144 - 0x0145 - New Licensee Code
    u8 sgb_flag; // 0x0146 - Super Game Boy Functionality
    u8 type; // 0x0147 - Cartridge Type
    u8 rom_size; // 0x0148 - ROM Size (how much is on the cart)
    u8 ram_size; // 0x0149 - RAM Size (how much is on the cart)
    u8 dest_code; // 0x014A - Destination Code
    u8 lic_code; // 0x014B - Old Licensee Code
    // If the Old License Code is 0x33, this is the 
    // indication that a New licensee code must be used.

    u8 version; // 0x014C - Version Number (usually 0)
    u8 checksum; // 0x014D - Checksum 
    // Checksum must match this verification algorithm:
    // uint8_t checksum = 0;
    // for (uint16_t address = 0x0134; address <= 0x014C; address++) {
    //     checksum = checksum - rom[address] - 1;
    // } 

    u16 global_checksum; // 0x014E - 0x014F - This 
    // checksum is not used except for Pokemon Stadium's 
    // emulator, so it is not really used here.
} rom_header;

bool cart_load(char *cart);

u8 cart_read(u16 address);
void cart_write(u16 address, u8 value);

bool cart_need_save();
void cart_battery_load();
void cart_battery_save();