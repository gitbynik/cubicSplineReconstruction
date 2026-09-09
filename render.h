#ifndef RENDER_H
#define RENDER_H

#include <SDL3/SDL.h>
#include "canvas.h"
#include "widgets.h"

SDL_Texture* iniFrameTexture(SDL_Renderer*,Canvas);
SDL_Texture* iniBGTexture(SDL_Renderer*,Canvas);
void FullScreenDisplayModeChange(SDL_Window*);
SDL_Texture* createHollowPtrTexture(float r,SDL_Renderer* renderer);
void renderAxis(Application);
void drawThickRectangle(SDL_Renderer* renderer, SDL_FRect r, int thickness);

#endif