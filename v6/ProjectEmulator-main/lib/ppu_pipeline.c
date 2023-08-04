#include <ppu.h>
#include <lcd.h>
#include <bus.h>


bool window_visible() {
    return LCDC_WIN_ENABLE && lcd_get_context()->win_x >= 0 && lcd_get_context()->win_x < 166 &&
    lcd_get_context()->win_y >= 0 && lcd_get_context()->win_y < YRES;
}


void pixel_fifo_push(u32 value) {
    // Pushes a pixel to the fifo
    fifo_entry *next = malloc(sizeof(fifo_entry));
    next->next = NULL;
    next->value = value;
    // Linked-list style implementation

    if (!ppu_get_context()->pfc.pixel_fifo.head) {
            // If not set, this is the first entry
        ppu_get_context()->pfc.pixel_fifo.head = ppu_get_context()->pfc.pixel_fifo.tail = next;
    }
    else {
        
        ppu_get_context()->pfc.pixel_fifo.tail->next = next; 
            // append next to the back of the fifo
        ppu_get_context()->pfc.pixel_fifo.tail = next;
    }
  
    ppu_get_context()->pfc.pixel_fifo.size++;
}

u32 pixel_fifo_pop() {
    // Pop a pixel from the fifo
    if (ppu_get_context()->pfc.pixel_fifo.size <= 0) {
        // If trying to pop when size is less than 1, something is wrong.
        fprintf(stderr, "ERR IN PIXEL FIFO!\n");
        exit(-8);
    }

    fifo_entry *popped = ppu_get_context()->pfc.pixel_fifo.head;
    ppu_get_context()->pfc.pixel_fifo.head = popped->next;
    ppu_get_context()->pfc.pixel_fifo.size--;
        // Pop the head, set the new head, decrement size
    u32 val = popped->value;
    free(popped);
    return val;
}

u32 fetch_sprite_pixels(int bit, u32 color, u8 bg_color) {

    for (int i = 0;i < ppu_get_context()->fetched_entry_count;i++) {
            // Loop through the number of fetched entries
        int sp_x = (ppu_get_context()->fetched_entries[i].x - 8) +
            ((lcd_get_context()->scroll_x % 8));
            // Get sprite x value
        if (sp_x + 8 < ppu_get_context()->pfc.fifo_x) {
            // past pixel point already
            continue;
        }

        int offset = ppu_get_context()->pfc.fifo_x - sp_x;
            // The offset of where the pixel goes

        if (offset < 0 || offset > 7) {
            // out of bounds
            continue;
        }

        bit = (7 - offset);
            // Current bit working on because we are going in reverse order

        if (ppu_get_context()->fetched_entries[i].f_x_flip) {
                // If flipped on the x axis then it is mirrored
            bit = offset;
        }

        u8 hi = !!(ppu_get_context()->pfc.fetch_entry_data[i * 2] & (1 << bit));
        u8 lo = !!(ppu_get_context()->pfc.fetch_entry_data[(i * 2) + 1] & (1 << bit)) << 1;
            // grab each bit from each byte to get the color

        bool bg_priority = ppu_get_context()->fetched_entries[i].f_bgp;
            // Check if the background should get priority

        if (!(hi|lo)) {
            // transparent
            continue;
        }

        if (!bg_priority || bg_color == 0) {
            // If background does not have priority or if background is transparent
            color = (ppu_get_context()->fetched_entries[i].f_pn) ?
                lcd_get_context()->sp2_colors[hi|lo] : lcd_get_context()->sp1_colors[hi|lo];
                // Set the color to the value from the palette either sp1 or sp2

            if (hi|lo) {
                break;
            }
        }
    }

    return color;
}

bool pipeline_fifo_add() {
    if (ppu_get_context()->pfc.pixel_fifo.size > 8) {
        // If the fifo is full already we can't add
        return false;
    }

    int x = ppu_get_context()->pfc.fetch_x - (8 - (lcd_get_context()->scroll_x % 8));
    for (int i = 0;i < 8;i++) {
        int bit = 7 - i;
        u8 hi = !!(ppu_get_context()->pfc.bgw_fetch_data[1] & (1 << bit));
        u8 lo = !!(ppu_get_context()->pfc.bgw_fetch_data[2] & (1 << bit)) << 1;
        u32 color = lcd_get_context()->bg_colors[hi | lo];
            // Color is based on the same bit in two bytes
            // hi and lo are actually named opposite on accident

        if (!LCDC_BGW_ENABLE) {
                // If the background not enabled, grab color from bg_color
            color = lcd_get_context()->bg_colors[0];
        }

        if (LCDC_OBJ_ENABLE) {
            // If sprites are enabled, look up the color from the sprite
            color = fetch_sprite_pixels(bit, color, hi | lo);
        }

        if (x >= 0) {
            pixel_fifo_push(color);
                // Push the pixel color to the fifo (happens each iteration)
            ppu_get_context()->pfc.fifo_x++;
        }
    }
    return true;
        // Allows FS_PUSH to move on
}

void pipeline_load_sprite_tile() {
    // Loads a sprite tile from memory
    oam_line_entry *le = ppu_get_context()->line_sprites;
    while (le) {
        // Loop through line_sprites linked-list
        int sp_x = (le->entry.x - 8) + (lcd_get_context()->scroll_x % 8);
            // Fetch sprite x value

        if ((sp_x >= ppu_get_context()->pfc.fetch_x && 
            sp_x < ppu_get_context()->pfc.fetch_x + 8) || 
            ((sp_x + 8) >= ppu_get_context()->pfc.fetch_x &&
            (sp_x + 8) < ppu_get_context()->pfc.fetch_x + 8)) {
                // If sprite x is within fetch_x, need to add entry
            
            ppu_get_context()->fetched_entries[ppu_get_context()->fetched_entry_count++] = le->entry;
        }

        le = le->next;
            // Iterate linked-list

        if (!le || ppu_get_context()->fetched_entry_count >= 3) {
            // max checking 3 sprites on 8-pixel section
            break;
        }
    }
}

void pipeline_load_sprite_data(u8 offset) {
    int cur_y = lcd_get_context()->ly;
    u8 sprite_height = LCDC_OBJ_HEIGHT;

    for (int i = 0;i < ppu_get_context()->fetched_entry_count;i++) {
        // Loop through fetched sprite entries
        u8 ty = ((cur_y + 16) - ppu_get_context()->fetched_entries[i].y) * 2;
            // tile y

        if (ppu_get_context()->fetched_entries[i].f_y_flip) {
            // If the tile is flipped upside down
            ty = ((sprite_height * 2) - 2) - ty;
        }

        u8 tile_index = ppu_get_context()->fetched_entries[i].tile;

        if (sprite_height == 16) {
            tile_index &= ~(1);
                // Remove the last bit, not needed
        }

        ppu_get_context()->pfc.fetch_entry_data[(i * 2) + offset] =
            bus_read(0x8000 + (tile_index * 16) + ty + offset);
            // set the data to read from memory location of tile
    }
}

void pipeline_load_window_tile() {
    if (!window_visible()) {
        return;
    }

    u8 window_y = lcd_get_context()->win_y;

    if (ppu_get_context()->pfc.fetch_x + 7 >= lcd_get_context()->win_x && 
        ppu_get_context()->pfc.fetch_x + 7 < lcd_get_context()->win_x + YRES + 14) {
        if (lcd_get_context()->ly >= window_y && lcd_get_context()->ly < window_y + XRES) {
            u8 w_tile_y = ppu_get_context()->window_line / 8;

            ppu_get_context()->pfc.bgw_fetch_data[0] = bus_read(LCDC_WIN_MAP_AREA + 
            ((ppu_get_context()->pfc.fetch_x + 7 - lcd_get_context()->win_x) / 8) +
            (w_tile_y * 32));

            if (LCDC_BGW_DATA_AREA == 0x8800) {
                ppu_get_context()->pfc.bgw_fetch_data[0] += 128;
            }
        }
    }
}

void pipeline_fetch() {

    switch (ppu_get_context()->pfc.cur_fetch_state) {
        case FS_TILE: {
            // Fetching the current tile for the location on the map
            ppu_get_context()->fetched_entry_count = 0;
                // Reset value to 0
            if (LCDC_BGW_ENABLE) {
                // If the background window is enabled
                ppu_get_context()->pfc.bgw_fetch_data[0] = 
                    bus_read(LCDC_BG_MAP_AREA + (ppu_get_context()->pfc.map_x / 8) + 
                    (((ppu_get_context()->pfc.map_y / 8)) * 32));
                // Read from the map area data + fifo map vals as offsets to get bgw fetch data

                if (LCDC_BGW_DATA_AREA == 0x8800) {
                    ppu_get_context()->pfc.bgw_fetch_data[0] += 128;
                }

                pipeline_load_window_tile();
                    // Picks the tile from the window of the window is enabled
            }

            if (LCDC_OBJ_ENABLE && ppu_get_context()->line_sprites) {
                // If object is enabled and there are line sprites to use
                pipeline_load_sprite_tile();
            }

            ppu_get_context()->pfc.cur_fetch_state = FS_DATA0;
                // Switch to the next state
            ppu_get_context()->pfc.fetch_x += 8;
                // Increment by 8 because we load 8 pixels of the tile
        } break;

        case FS_DATA0: {
            // Fetching Background window data
            ppu_get_context()->pfc.bgw_fetch_data[1] = 
                bus_read(LCDC_BGW_DATA_AREA + (ppu_get_context()->pfc.bgw_fetch_data[0] * 16) + 
                ppu_get_context()->pfc.tile_y);
            // Setting byte 1 of background data area from the data fetched in FS_TILE

            pipeline_load_sprite_data(0);

            ppu_get_context()->pfc.cur_fetch_state = FS_DATA1;
                // Move to the next state
        } break;

        case FS_DATA1: {
            ppu_get_context()->pfc.bgw_fetch_data[2] = 
                bus_read(LCDC_BGW_DATA_AREA + (ppu_get_context()->pfc.bgw_fetch_data[0] * 16) + 
                ppu_get_context()->pfc.tile_y + 1);
            // Grab the next byte

            pipeline_load_sprite_data(1);


            ppu_get_context()->pfc.cur_fetch_state = FS_IDLE;
                // Go to the idle state
        } break;
        case FS_IDLE: {
            ppu_get_context()->pfc.cur_fetch_state = FS_PUSH;
                // Just go to the next state
        } break;
        case FS_PUSH: {
            if (pipeline_fifo_add()) {
                // PPU keeps trying to add pixels until it succeeds
                ppu_get_context()->pfc.cur_fetch_state = FS_TILE;
            }
        } break;

    }


}

void pipeline_push_pixel() {
    if (ppu_get_context()->pfc.pixel_fifo.size > 8) {
        // If the fifo is full, pop a pixel off
        u32 pixel_data = pixel_fifo_pop();

        if (ppu_get_context()->pfc.line_x >= (lcd_get_context()->scroll_x % 8)) {

                ppu_get_context()->video_buffer[ppu_get_context()->pfc.pushed_x + (lcd_get_context()->ly * XRES)] = pixel_data;
                    // Add pixel data to the video buffer
                ppu_get_context()->pfc.pushed_x++;
        }

        ppu_get_context()->pfc.line_x++;
    }
}

void pipeline_process() {

    ppu_get_context()->pfc.map_y = (lcd_get_context()->ly + lcd_get_context()->scroll_y);
        // absolute location on the map y axis
    ppu_get_context()->pfc.map_x = (ppu_get_context()->pfc.fetch_x + lcd_get_context()->scroll_x);
        // absolute location on the map x axis
    ppu_get_context()->pfc.tile_y = ((lcd_get_context()->ly + lcd_get_context()->scroll_y) % 8) * 2;
        // get the y value on current tile
    if (!(ppu_get_context()->line_ticks & 1)) {
            // If the first bit is not set it is an even line, so pipeline_fetch. happens every other tick.
        pipeline_fetch();
    }
    pipeline_push_pixel();
}

void pipeline_fifo_reset() {
    // Called when done with the fifo
    while (ppu_get_context()->pfc.pixel_fifo.size) {
        pixel_fifo_pop();
    }
    // Just pop everything until it is empty

    ppu_get_context()->pfc.pixel_fifo.head = 0;
}