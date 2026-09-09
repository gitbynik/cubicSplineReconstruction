#include "render.h"
#include "canvas.h"
#include "math.h"
#include <SDL3/SDL.h>
#include <stdlib.h>

SDL_Texture* iniBGTexture(SDL_Renderer * renderer,Canvas c)
{
	SDL_Texture* bgtexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, c->width, c->height);
	SDL_SetRenderTarget(renderer, bgtexture);
	SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);

	SDL_FRect r1 = {0,0,c->width,c->height};
	SDL_RenderFillRect(renderer,&r1);
	SDL_SetRenderTarget(renderer, NULL);
	return bgtexture;
}

SDL_Texture* iniFrameTexture(SDL_Renderer* renderer,Canvas c)
{
	SDL_Texture* frametexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING, c->width, c->height);
	SDL_UpdateTexture(frametexture, NULL, c->CanvasBuffer, c->pitch);
	return frametexture;
}

void FullScreenDisplayModeChange(SDL_Window * window)
{
	int* displaymodecount = NULL; //the number of resolutions available
	int* displaycount = NULL; //the number of gpus/displays i guess??;
	SDL_DisplayID * displayID = SDL_GetDisplays(displaycount);
	SDL_DisplayMode ** modeptr = SDL_GetFullscreenDisplayModes(
	displayID[0],displaymodecount); //use 0 for default graphics 1 for secondary gpu
	SDL_SetWindowFullscreenMode(window,modeptr[0]); //pass this to setfullscreen
}

SDL_Texture* createHollowPtrTexture(float r, SDL_Renderer* renderer)
{
	Uint32 * pixels = (Uint32*) malloc(4*r*r*sizeof(Uint32));
	for(int i=0;i<2*r;i++)
	{
		for(int j=0;j<2*r;j++)
		{
			float rcalc = sqrt((i-r)*(i-r) + (j-r)*(j-r));
			if(rcalc <= r && rcalc >= r-6)
				{pixels[i+j*(int)(2*r)] = 0xc0c1c2ff;}
			else
				{pixels[i+j*(int)(2*r)] = 0x00000000;}
		}
	}
	SDL_Surface* surface = SDL_CreateSurfaceFrom(2*r, 2*r, SDL_PIXELFORMAT_RGBA8888,pixels, 2*r*sizeof(Uint32));
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
	free(pixels);
	return texture;
}

void renderAxis(Application App)
{
	SDL_SetRenderDrawColor(App->renderer, 100, 100, 100, 255);

	SDL_FRect ryaxis = {App->c->width/2.0-5+App->c->originx,100,10,App->c->height-200};
	SDL_RenderFillRect(App->renderer,&ryaxis);

	SDL_FRect rxaxis = {100,App->c->height/2.0-5-App->c->originy,App->c->width-200,10};
	SDL_RenderFillRect(App->renderer,&rxaxis);
}

void drawThickRectangle(SDL_Renderer* renderer, SDL_FRect r, int thickness)
{
	SDL_FRect r1 = {r.x,r.y,r.w,thickness};
	SDL_FRect r2 = {r.x,r.y,thickness,r.h};
	SDL_FRect r3 = {r.x+r.w-thickness,r.y,thickness,r.h};
	SDL_FRect r4 = {r.x,r.y-thickness+r.h,r.w,thickness};

	SDL_RenderFillRect(renderer, &r1);
	SDL_RenderFillRect(renderer, &r2);
	SDL_RenderFillRect(renderer, &r3);
	SDL_RenderFillRect(renderer, &r4);
}