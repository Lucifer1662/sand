#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <iostream>

#include "RenderContext.h"
#include "world.h"

SDL_Window* window;

SDL_Point center = {.x = 100, .y = 100};

RenderContext ctx;
Camera camera;

World world;

void redraw() {
    SDL_SetRenderDrawColor(ctx.renderer, /* RGBA: black */ 0x00, 0x00, 0x00,
                           0xFF);
    SDL_RenderClear(ctx.renderer);

    world.draw(ctx);

    SDL_RenderPresent(ctx.renderer);
}

uint32_t ticksForNextKeyDown = 0;

bool holding = false;

void square(int x, int y, Atom atom, int size) {
    for (int of_x = 0; of_x < size; of_x++) {
        for (int of_y = 0; of_y < size; of_y++) {
            world.set(x + of_x, y + of_y, atom);
        }
    }
}

bool handle_events() {
    world.update();

    SDL_Event event;
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT) {
        return false;
    }

    SDL_PumpEvents();
    if (event.type == SDL_MOUSEBUTTONUP) holding = false;
    if (event.type == SDL_MOUSEBUTTONDOWN) holding = true;

    if (holding) {
        int x, y;
        Uint32 buttons = SDL_GetMouseState(&x, &y);
        camera.toWorld(x, y);
        if (buttons == 1) square(x, y, Sand, 5);
        if (buttons == 4) square(x, y, Stone, 5);
    }

    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                camera.addY(1);
                break;
            case SDLK_DOWN:
                camera.addY(-1);
                break;
            case SDLK_RIGHT:
                camera.addX(1);
                break;
            case SDLK_LEFT:
                camera.addX(-1);
                break;
        }
    }

    world.swapBuffers();

    redraw();
    return true;
}

void run_main_loop() {
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop([]() { handle_events(); }, 0, true);
#else
    while (handle_events())
        ;
#endif
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    int screenX = 300, screenY = 300;

    SDL_CreateWindowAndRenderer(screenX, screenY, 0, &window, &ctx.renderer);

    ctx.setScreenSize(screenX, screenY);
    ctx.setCamera(&camera);

    redraw();
    run_main_loop();

    SDL_DestroyRenderer(ctx.renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}