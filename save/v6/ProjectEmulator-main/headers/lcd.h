#pragma once

#include <common.h>

typedef struct {

    // registers ...

    u8 lcdc; // 0xFF40 - LCD Control
    u8 lcds; // 0xFF41 - LCD Status
    u8 scroll_y; // 0xFF42
    u8 scroll_x; // 0xFF43
    u8 ly; // 0xFF44 - LCD Y Coordinate, indicates the current horizontal line
    u8 ly_compare; // 0xFF45 - LYC LY Compare
    u8 dma;
    u8 bg_palette;
    u8 obj_palette[2];
    u8 win_y;
    u8 win_x;

    u32 bg_colors[4];
    u32 sp1_colors[4];
    u32 sp2_colors[4];

} lcd_context;

typedef enum {

    MODE_HBLANK,
    MODE_VBLANK,
    MODE_OAM,
    MODE_XFER

} lcd_mode;

lcd_context *lcd_get_context();

// LCDC Bit Macros:
#define LCDC_BGW_ENABLE (BIT(lcd_get_context()->lcdc,0)) 
// bit 0: BG and Window enable/priority	0=Off, 1=On
#define LCDC_OBJ_ENABLE (BIT(lcd_get_context()->lcdc,1))
// bit 1: OBJ enable	0=Off, 1=On
#define LCDC_OBJ_HEIGHT (BIT(lcd_get_context()->lcdc,2) ? 16 : 8)
// bit 2: OBJ size	0=8×8, 1=8×16
#define LCDC_BG_MAP_AREA (BIT(lcd_get_context()->lcdc, 3) ? 0x9C00 : 0x9800)
// bit 3: BG tile map area	0=9800-9BFF, 1=9C00-9FFF
#define LCDC_BGW_DATA_AREA (BIT(lcd_get_context()->lcdc, 4) ? 0x8000 : 0x8800)
// bit 4: BG and Window tile data area	0=8800-97FF, 1=8000-8FFF
#define LCDC_WIN_ENABLE (BIT(lcd_get_context()->lcdc, 5))
// bit 5: Window enable	0=Off, 1=On
#define LCDC_WIN_MAP_AREA (BIT(lcd_get_context()->lcdc, 6) ? 0x9C00 : 0x9800)
// bit 6: Window tile map area	0=9800-9BFF, 1=9C00-9FFF
#define LCDC_LCD_ENABLE (BIT(lcd_get_context()->lcdc, 7))
// bit 7: LCD and PPU enable	0=Off, 1=On


// LCD Status Bits:
#define LCDS_MODE ((lcd_mode)(lcd_get_context()->lcds & 0b11))
#define LCDS_MODE_SET(mode) { lcd_get_context()->lcds &= ~0b11; lcd_get_context()->lcds |= mode; }

#define LCDS_LYC (BIT(lcd_get_context()->lcds, 2))
#define LCDS_LYC_SET(b) (BIT_SET(lcd_get_context()->lcds, 2, b))
/* 
Bit 6 - LYC=LY STAT Interrupt source         (1=Enable) (Read/Write)
Bit 5 - Mode 2 OAM STAT Interrupt source     (1=Enable) (Read/Write)
Bit 4 - Mode 1 VBlank STAT Interrupt source  (1=Enable) (Read/Write)
Bit 3 - Mode 0 HBlank STAT Interrupt source  (1=Enable) (Read/Write)
Bit 2 - LYC=LY Flag                          (0=Different, 1=Equal) (Read Only)
Bit 1-0 - Mode Flag                          (Mode 0-3, see below) (Read Only)
          0: HBlank
          1: VBlank
          2: Searching OAM
          3: Transferring Data to LCD Controller
*/

typedef enum {
    SS_HBLANK = (1 << 3),
    SS_VBLANK = (1 << 4),
    SS_OAM = (1 << 5),
    SS_LYC = (1 << 6),
} stat_src;

#define LCDS_STAT_INT(src) (lcd_get_context()->lcds & src)
// Used to tell if an interrupt source is enabled

void lcd_init();

u8 lcd_read(u16 address);
void lcd_write(u16 address, u8 value);
