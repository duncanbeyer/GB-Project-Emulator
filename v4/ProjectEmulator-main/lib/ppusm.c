#include <ppu.h>
#include <lcd.h>
#include <cpu.h>
#include <interrupts.h>

void pipeline_fifo_reset();
void pipeline_process();


void increment_ly() {
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


void ppu_mode_oam() {
    if (ppu_get_context()->line_ticks >= 80) {
        LCDS_MODE_SET(MODE_XFER);
        // In OAM scan, after 80 dots, the LCD jumps to mode 3

        ppu_get_context()->pfc.cur_fetch_state = FS_TILE;
        ppu_get_context()->pfc.line_x = 0;
        ppu_get_context()->pfc.fetch_x = 0;
        ppu_get_context()->pfc.pushed_x = 0;
        ppu_get_context()->pfc.fifo_x = 0;
    }
}

void ppu_mode_xfer() {
     pipeline_process();

    if (ppu_get_context()->pfc.pushed_x >= XRES) {
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

                printf("FPS: %dld\n", fps);
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