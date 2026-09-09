#include "canvas.h"
#include "render.h"
#include "spline.h"
#include <stdlib.h>
#include <math.h>
#include "update.h"
#include <SDL3/SDL.h>

int dtod(int x, int y, Canvas c)	{return x+y*c->width;}

Canvas CanvasInit(SDL_Window* window,SDL_Renderer * renderer)
{
	Canvas c = malloc(sizeof(struct canvas));
	c->width=2560;
	c->height=1440;
	c->CanvasBuffer = (Uint32*) calloc(c->width*c->height,sizeof(Uint32));
	c->pitch = c->width*sizeof(Uint32);
	c->xlim = 10.5;
	c->ylim = c->xlim * (c->height/2.0-100)/(c->width/2.0-100);
	c->scale = (c->width/2.0 - 100)/c->xlim;//100 IS THE FLIPPING BORDER
	c->originx = 0;
	c->originy = 0;
	c->frameTexture = iniFrameTexture(renderer,c);
	SDL_SetTextureBlendMode(c->frameTexture, SDL_BLENDMODE_BLEND);
	c->bgTexture = iniBGTexture(renderer,c);
	c->canvasRequiresReset = false;
	c->saveRequested = false;
	return c;
}

void destroyCanvas(Canvas c)
{
	SDL_DestroyTexture(c->bgTexture);
	if(c->frameTexture) SDL_DestroyTexture(c->frameTexture);
	free(c->CanvasBuffer);
	free(c);
}

void drawCircle(float x, float y, int r, Canvas c,Uint32 colour)
{
	for(int i=x-r;i<x+r;i++)
	{
		if(i<0||i>=c->width) continue;
		for(int j=y-r;j<y+r;j++)
		{
			if(j<0||j>=c->height) continue;
			if(sqrt((i-x)*(i-x) + (j-y)*(j-y)) <=r) c->CanvasBuffer[dtod(i,j,c)] = colour;
		}
	}
}

void drawSquare(int x, int y, int r, Canvas c,Uint32 colour)
{
	for(int i=x-r;i<x+r;i++)
	{
		if(i<0||i>=c->width) continue;
		for(int j=y-r;j<y+r;j++)
		{
			if(j<0||j>=c->height) continue;
			c->CanvasBuffer[dtod(i,j,c)] = colour;
		}
	}
}

void drawCubics(Canvas c, plotData* pd)
{
	memset(c->CanvasBuffer, 0, c->width *c->height * sizeof(Uint32));
	float scale = c->xlim/(c->width/2.0 - 100);
	for(int k = 0;k<pd->dp->num-1;k++)
	{
		for(double t=0;t<pd->dp->h.data[k];t+=0.01)
		{
			Cubic cX = pd->CubicArrayX.data[k];
			Cubic cY = pd->CubicArrayY.data[k];

			double x = ((cX.a * t + cX.b) * t + cX.c) * t + cX.d;
			double y = ((cY.a * t + cY.b) * t + cY.c) * t + cY.d;

			graphToScreenCords(x, y, &x, &y, c);
			if (y > 100 && y < c->height-100 && x > 100 && x < c->width-100) drawCircle(x, y, 2, c, 0xffffffff);
		}
	}
	SDL_UpdateTexture(c->frameTexture, NULL, c->CanvasBuffer, c->pitch);
}

void graphToScreenCords(double graphX, double graphY, double* screenX, double* screenY, Canvas c)
{
	*screenX = c->width/2.0 - c->originx + graphX*c->scale;
	*screenY = c->height/2.0 - c->originy - graphY*c->scale;
}

void screenToGraphCords(double screenX, double screenY, double* graphX, double* graphY, Canvas c)
{
	*graphX = (screenX - c->width/2.0 + c->originx)/c->scale;
	*graphY = (-screenY + c->height/2.0 - c->originy)/c->scale;
}