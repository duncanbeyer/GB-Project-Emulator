#include <ui.h>
#include <emu.h>
#include <bus.h>
#include <../sdl2_lib/include/SDL.h>
#include <../sdl2_lib/include/SDL_ttf.h>
#include <pthread.h>

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;
SDL_Surface *screen;

SDL_Window *sdlDebugWindow;
SDL_Renderer *sdlDebugRenderer;
SDL_Texture *sdlDebugTexture;
SDL_Surface *debugScreen;

static int scale = 2;

void ui_init() {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error: %s.\n", SDL_GetError());
    }
    printf("SDL_INIT\n");
    TTF_Init();
    printf("TTF_INIT\n");

    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0, &sdlWindow, &sdlRenderer) != 0) {
        printf("Error: %s.\n", SDL_GetError());
    }

    //                          16 tiles x 32 tiles, each 8 pixels wide
    SDL_CreateWindowAndRenderer(16 * 8 * scale, 32 * 8 * scale, 0, 
    &sdlDebugWindow, &sdlDebugRenderer);

    // Debug Window Creation Information

    debugScreen = SDL_CreateRGBSurface(0, (16 * 8 * scale) + (16 * scale),
    (32 * 8 * scale) + (64 * scale), 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);


    sdlDebugTexture = SDL_CreateTexture(sdlDebugRenderer, 
    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
    (16 * 8 * scale) + (16 * scale),
    (32 * 8 * scale) + (64 * scale));
    
    int x, y;
    
    SDL_GetWindowPosition(sdlWindow, &x, &y);
    SDL_SetWindowPosition(sdlDebugWindow, x + (SCREEN_WIDTH/2) + 10, y);
    // Arrange the Debug window to sit next to the game window

}

void delay(u32 ms) {
    SDL_Delay(ms);
}

u32 get_ticks() {
    return SDL_GetTicks();
}

static unsigned long tile_colors[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void display_tile(SDL_Surface *surface, u16 startLocation, u16 tileNum, int x, int y) {
    SDL_Rect rc;

    for (int tileY = 0;tileY < 16;tileY += 2) {
    // += 2 because each line is 2 bytes
        u8 b1 = bus_read(startLocation + (tileNum * 16) + tileY);
        u8 b2 = bus_read(startLocation + (tileNum * 16) + tileY + 1);
        for (int bit = 7;bit >= 0; bit--) {
            u8 hi = !!(b1 & (1 << bit)) << 1;
            u8 lo = !!(b2 & (1 << bit));
            // The combination of the same bit position of the different bytes 
            // determines the color of the pixel.

            u8 color = hi | lo;

            rc.x = x + ((7 - bit) * scale);
            rc.y = y + (tileY/2 * scale);
            rc.w = scale;
            rc.h = scale;

            SDL_FillRect(surface, &rc, tile_colors[color]);
        }
    }
}

void update_debug_window() {
    int xDraw = 0;
    int yDraw = 0;
    int tileNum = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = debugScreen->w;
    rc.h = debugScreen->h;
    SDL_FillRect(debugScreen, &rc, 0xFF111111);

    u16 addr = 0x8000; // The start of tile data

    // 384 tiles, 24 x 16
    for (int y = 0; y < 24; y++) {
        for (int x = 0;x < 16;x++) {
            display_tile(debugScreen, addr, tileNum, xDraw + (x * scale), yDraw + (y * scale));
            xDraw += (8 * scale);
            tileNum++;
        }
        yDraw += (8 * scale);
        xDraw = 0;
    }

    SDL_UpdateTexture(sdlDebugTexture, NULL, debugScreen->pixels, debugScreen->pitch);
	SDL_RenderClear(sdlDebugRenderer);
	SDL_RenderCopy(sdlDebugRenderer, sdlDebugTexture, NULL, NULL);
	SDL_RenderPresent(sdlDebugRenderer);

}

void ui_update() {
    update_debug_window();
}

void ui_handle_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0) {
        //TODO SDL_UpdateWindowSurface(sdlWindow);
        //TODO SDL_UpdateWindowSurface(sdlTraceWindow);
        //TODO SDL_UpdateWindowSurface(sdlDebugWindow);

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            emu_get_context()->die = true;
        }
    }
}