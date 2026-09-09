#ifndef CANVAS_H
#define CANVAS_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include "spline.h"

struct canvas
{
	int width,height,pitch;
	Uint32 * CanvasBuffer;
	bool canvasRequiresReset,saveRequested;
	SDL_Texture* frameTexture, *bgTexture;
	float xlim,ylim,originx,originy;
	double scale;
};
typedef struct canvas* Canvas;

Canvas CanvasInit(SDL_Window*, SDL_Renderer*);
void drawCircle(float centrex, float centrey,int radius,Canvas,Uint32);
void drawSquare(int x, int y, int r, Canvas c,Uint32 colour);
void destroyCanvas(Canvas);
void drawCubics(Canvas, plotData* pd); //returns the time took to complete
void graphToScreenCords(double graphX, double graphY, double* screenX, double* screenY, Canvas c);
void screenToGraphCords(double screenX, double screenY, double* graphX, double* graphY, Canvas c);

#endif