#include "update.h"
#include "canvas.h"
#include "widgets.h"
#include "stdio.h"
#include "app.h"
#include "math.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

Uint64 bgcolor = 0x191919ff;

void poller(SDL_Event e, Application App)
{
	switch(e.type)
	{
		case SDL_EVENT_QUIT:
			App->running = false;
			break;
		case SDL_EVENT_KEY_DOWN:
			App->modmask = SDL_GetModState();
			keydowncontrols(e,App);
			break;
		case SDL_EVENT_KEY_UP:
			App->modmask = SDL_GetModState(); //this is for modifier keys, 
			//maybe do this in key down as well?
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			App->mousedown=true;
			break;		
		case SDL_EVENT_MOUSE_BUTTON_UP:
			if(App->modmask & SDL_KMOD_LCTRL) App->createPoint = true;
			App->mousedown=false;
			break;
		case SDL_EVENT_MOUSE_MOTION:
			App->oldMouse.x = App->Mouse.x;
			App->oldMouse.y = App->Mouse.y;
			SDL_GetMouseState(&App->Mouse.x,&App->Mouse.y);
			SDL_RenderCoordinatesFromWindow(App->renderer, App->Mouse.x,  App->Mouse.y, &App->Mouse.x,  &App->Mouse.y);
			/*if(App->mousedown && App->Mouse.x < App->c->width-600 && !App->anyButtonPressed)
			{
				App->c->originy-= App->Mouse.y - App->oldMouse.y;
				App->c->originx+= App->Mouse.x - App->oldMouse.x;
				App->c->canvasRequiresReset = true;
			}*/
			break;
		case SDL_EVENT_MOUSE_WHEEL: //zooms in and out, centered at origin
			/*App->c->xlim-=e.wheel.y;
			App->c->ylim = App->c->xlim * (App->c->height/2.0-100)/(App->c->width/2.0-100);
			App->c->scale = (App->c->width/2.0 - 100)/App->c->xlim;
			App->c->canvasRequiresReset = true;*/
			break;
	}
}

void handlefulls(SDL_Window *window,bool* fulls)
{
	if(!*fulls) SDL_SetWindowFullscreen(window,*fulls = true);
	else
	{
		SDL_HideWindow(window);
		SDL_SetWindowFullscreen(window,*fulls = false);
		SDL_MaximizeWindow(window);
		SDL_ShowWindow(window);
	}
}

void saveCurrentFrame(Canvas c)
{
	SDL_Surface* FrameScreenshot = SDL_CreateSurfaceFrom(c->width, c->height, SDL_PIXELFORMAT_RGBA8888, c->CanvasBuffer, c->pitch);
	IMG_SavePNG(FrameScreenshot, "image.png");
	SDL_DestroySurface(FrameScreenshot);
	c->saveRequested = false;
}

void keydowncontrols(SDL_Event event,Application App)
{
	if(event.key.key == SDLK_F11) handlefulls(App->window,&App->fulls);
	switch(event.key.key)
	{
		case SDLK_S:
			if(App->modmask & SDL_KMOD_LCTRL) 
			{
				App->saveRequested = true;
				App->c->saveRequested = true;
			}break;
		case SDLK_LEFT:
			App->c->originx-=10;
			App->c->canvasRequiresReset = true;
			break;
		case SDLK_RIGHT:
			App->c->originx+=10;
			App->c->canvasRequiresReset = true;
			break;
		case SDLK_UP:
			App->c->originy+=10;
			App->c->canvasRequiresReset = true;
			break;
		case SDLK_DOWN:
			App->c->originy-=10;
			App->c->canvasRequiresReset = true;
			break;
		case SDLK_P: App->pointRender = !App->pointRender;break;
		case SDLK_L: 
			if(App->modmask & SDL_KMOD_LCTRL) App->loadRequested = true;
			else App->lineRender = !App->lineRender;break;
		case SDLK_I: App->bgimageRender = !App->bgimageRender;break;
	}
}

void saveData(DataPoints* dp, const char* filename)
{
	FILE* f = fopen(filename, "w");
	if(f == NULL)return;
	fprintf(f, "%d\n", dp->num);

	for(int i = 0; i < dp->num; i++)fprintf(f, "%.17g %.17g\n",dp->x.data[i],dp->y.data[i]);
	fclose(f);
}

void loadData(DataPoints* dp, const char* filename)
{
	FILE* f = fopen(filename, "r");
	if(f == NULL)return;

	int num;
	if(fscanf(f, "%d", &num) != 1)
	{
		fclose(f);
		return;
	}

	destroyData(dp);

	initDoubleVector(&dp->x);
	initDoubleVector(&dp->y);
	initDoubleVector(&dp->h);

	dp->num = 0;
	for(int i = 0; i < num; i++)
	{
		double x, y;
		if(fscanf(f, "%lf %lf", &x, &y) != 2) break;
		addDataPoint(dp, x, y);
	}

	fclose(f);
}