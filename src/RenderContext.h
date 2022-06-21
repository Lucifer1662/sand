#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

struct SDL_Renderer;

class Camera {
    int x = 0, y = 0;

   public:
    bool isDirty = true;

    void setPos(int x, int y) {
        this->x = x;
        this->y = y;
    }

    void addX(int x) {
        this->x += x;
        isDirty = true;
    }
    void addY(int y) {
        this->y += y;
        isDirty = true;
    }

    int getX() { return x; }
    int getY() { return y; }

    void toScreenSpace(int& x, int& y) {
        x -= this->x;
        y -= this->y;
        y = -y;
    }

    void toWorld(int& x, int& y) {
        y = -y;

        x += this->x;
        y += this->y;
    }
};

class RenderContext {
    int screenSizeX, screenSizeY;
    Camera* camera;
    SDL_Texture* texTarget;


   public:
    SDL_Renderer* renderer;

   public:
    bool isVisible(int bl_x, int bl_y, int tr_x, int tr_y) { return true; }
    void setCamera(Camera* camera) { this->camera = camera; }
    void setScreenSize(int x, int y) {
        this->screenSizeX = x;
        this->screenSizeY = y;
    }

    bool forceDraw() { return camera->isDirty; }

    void preDrawToScreen() { 
        setColour(0,0,0,0xFF);
        SDL_SetRenderTarget(renderer, texTarget); 
        if(camera->isDirty){
            SDL_RenderClear(renderer);
        }    
    }

    void postDrawToScreen() {
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, texTarget, NULL, NULL);
        SDL_RenderPresent(renderer);

        camera->isDirty = false;
    }

    void init(){
        texTarget = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                  SDL_TEXTUREACCESS_TARGET, screenSizeX, screenSizeY);
    }

    void renderPoints(int x, int y) {
        camera->toScreenSpace(x, y);
        SDL_RenderDrawPointF(renderer, x, y);
    }
    void setColour(int r, int g, int b, int a) {
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
    }

    // right and top are inclusive
    void renderRectangle(int x, int y, int right, int top) {
        camera->toScreenSpace(x, y);
        camera->toScreenSpace(right, top);
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = right - x + 1;
        rect.h = top - y + 1;
        SDL_RenderDrawRect(renderer, &rect);
    }

    // right and top are inclusive
    void renderFillRectangle(int x, int y, int right, int top) {
        camera->toScreenSpace(x, y);
        camera->toScreenSpace(right, top);
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = right - x + 1;
        rect.h = top - y + 1;
        SDL_RenderFillRect(renderer, &rect);
    }

};