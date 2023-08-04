#include <ppu.h>
#include <lcd.h>
#include <cpu.h>
#include <interrupts.h>
#include <string.h>
#include <emu.h>
#include <cart.h>

void pipeline_fifo_reset();
void pipeline_process();
bool window_visible();


void increment_ly() {

    if (window_visible() && lcd_get_context()->ly >= lcd_get_context()->win_y &&
    lcd_get_context()->ly < lcd_get_context()->win_y + YRES) {
        ppu_get_context()->window_line++;
    }

    // Increment the current line
    lcd_get_context()->ly++;

    if (lcd_get_context()->ly == lcd_get_context()->ly_compare) {
        LCDS_LYC_SET(1);
        if (LCDS_STAT_INT(SS_LYC)) {
            cpu_request_interrupt(IT_LCD_STAT);
        }
    }
    else {
        LCDS_LYC_SET(0);
    }
}

void load_line_sprites() {
        // Load all the sprites for a given line
    int cur_y = lcd_get_context()->ly;
        // Current line (current y)

    u8 sprite_height = LCDC_OBJ_HEIGHT;

    memset(ppu_get_context()->line_entry_array, 0, 
        sizeof(ppu_get_context()->line_entry_array));
        // Set all values in line_entry_array to 0

    for (int i = 0;i < 40;i++) {
        oam_entry e = ppu_get_context()->oam_ram[i];
            // Loop through each OAM entry

        if (!e.x) {
            // x = 0 means not visible on screen
            continue;
        }

        if (ppu_get_context()->line_sprite_count >= 10) {
            // Max of 10 sprites per line
            break;
        }

        if (e.y <= cur_y + 16 && e.y + sprite_height > cur_y + 16) {
            // The sprite is on the current line

            oam_line_entry *entry = &ppu_get_context()->line_entry_array[
                ppu_get_context()->line_sprite_count++
            ];
                // Grab the entry from line_entry_array

            entry->entry = e;
                // set the sprite entry
            entry->next = NULL;

            if (!ppu_get_context()->line_sprites ||
                ppu_get_context()->line_sprites->entry.x > e.x) {
                    // If line sprites has not been initialized OR
                    // if the entry x > e.x
                entry->next = ppu_get_context()->line_sprites;
                ppu_get_context()->line_sprites = entry;
                continue;
                }


            // Otherwise do some sorting ...

            oam_line_entry *le = ppu_get_context()->line_sprites;
            oam_line_entry *prev = le;
            while (le) {
                if (le -> entry.x > e.x) {
                    prev->next = entry;
                    entry->next = le;
                    break;
                }

                if (!le->next) {
                    le->next = entry;
                    break;
                }

                prev = le;
                le = le->next;
            }
        }
    }
}


void ppu_mode_oam() {
        // Reading the tiles that are on the current line
    if (ppu_get_context()->line_ticks >= 80) {
        LCDS_MODE_SET(MODE_XFER);
            // In OAM scan, after 80 dots, the LCD jumps to mode 3

        ppu_get_context()->pfc.cur_fetch_state = FS_TILE;
        ppu_get_context()->pfc.line_x = 0;
        ppu_get_context()->pfc.fetch_x = 0;
        ppu_get_context()->pfc.pushed_x = 0;
        ppu_get_context()->pfc.fifo_x = 0;

    }
    if (ppu_get_context()->line_ticks == 1) {
            //read oam on the first tick only
        ppu_get_context()->line_sprites = 0;
        ppu_get_context()->line_sprite_count = 0;

        load_line_sprites();
    }
}

void ppu_mode_xfer() {
     pipeline_process();

    if (ppu_get_context()->pfc.pushed_x >= XRES) {
        // If all possible pixels have been pushed
        pipeline_fifo_reset();
        LCDS_MODE_SET(MODE_HBLANK);
        // In drawing pixels mode, once line tickets finishes, LCD jumps to HBlank
        if (LCDS_STAT_INT(SS_HBLANK)) {
            cpu_request_interrupt(IT_LCD_STAT);
        }
    }
}

void ppu_mode_vblank() {
    if (ppu_get_context()->line_ticks >= TICKS_PER_LINE) {
        // When we run out of ticks in the line
        increment_ly(); // Move to a new line
        
        if (lcd_get_context()->ly >= LINES_PER_FRAME) {
            // If we have reached the # of lines in a frame
            LCDS_MODE_SET(MODE_OAM);
            lcd_get_context()->ly = 0;
            ppu_get_context()->window_line = 0;
        }

        ppu_get_context()->line_ticks = 0;

    }
}

static u32 target_frame_time = 1000 / 60;
static long prev_frame_time = 0;
static long start_timer = 0;
static long frame_count = 0;

void ppu_mode_hblank() {

    if (ppu_get_context()->line_ticks >= TICKS_PER_LINE) {
        increment_ly();

        if (lcd_get_context()->ly >= YRES) {
            LCDS_MODE_SET(MODE_VBLANK);

            cpu_request_interrupt(IT_VBLANK);

            if (LCDS_STAT_INT(SS_VBLANK)) {
                cpu_request_interrupt(IT_LCD_STAT);
            }

            ppu_get_context()->current_frame++;

            // calculate FPS...
            u32 end = get_ticks();
            u32 frame_time = end - prev_frame_time;

            if (frame_time < target_frame_time) {
                delay(target_frame_time - frame_time);
            }

            if (end - start_timer >= 1000) {
                u32 fps = frame_count;
                start_timer = end;
                frame_count = 0;

                printf("FPS: %d\n", fps);


                if (cart_need_save()) {
                    cart_battery_save();
                }
            }

            frame_count++;
            prev_frame_time = get_ticks();
        }
        else {
            LCDS_MODE_SET(MODE_OAM);
        }

        ppu_get_context()->line_ticks = 0;
    }
}