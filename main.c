#include <SDL3/SDL.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "app.h"
#include "update.h"
#include "canvas.h"
#include "widgets.h"
#include "render.h"
#include "stdio.h"
#include "spline.h"

int main(int argc, char *argv[])
{
	int numx = 10;
	if(argc==2) numx = atoi(argv[1]);

	SDL_Event e;

	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS);
	TTF_Init();
	Uint64 current_render_time = 0;
	Application App = AppInit(60);
	SDL_FRect borderRect = {90,90,App->c->width-180,App->c->height-180 };

	plotData pd;
	createPlotData(&pd,numx);
	HollowPtrVector HPV1;
	initHollowPtrVector(&HPV1);

	for(int i=0;i<pd.dp->num;i++) 
	{
		double x,y;
		graphToScreenCords(pd.dp->x.data[i],pd.dp->y.data[i],&x,&y,App->c);
		pushHollowPtrVector(&HPV1,createHollowPtr(x,y,15,App->renderer,NULL));
	}

	Label drawTime = createLabel(435,20,App->TextSys,"0");
	Label l2 = createLabel(130,20,App->TextSys,"Time to draw             ms");
	updateLabelFromDouble(drawTime,calcRunTime(App, drawCubics, &pd));
	Label fpscounterlabel =createLabel(785,20,App->TextSys,"0");
	Label frametime =createLabel(1000,20,App->TextSys,"0");
	App->last_render_time = SDL_GetPerformanceCounter();

	while(App->running)
	{
		while(SDL_PollEvent(&e)) poller(e, App);

		if(App->loadRequested) 
		{
			loadData(pd.dp, "data.txt");
			destroyCubicVector(&pd.CubicArrayX);
			destroyCubicVector(&pd.CubicArrayY);
			initCubicVector(&pd.CubicArrayX);
			initCubicVector(&pd.CubicArrayY);

			for(int i = 0; i < pd.dp->num - 1; i++)
			{
				Cubic c = {0};
				pushCubicVector(&pd.CubicArrayX, c);
				pushCubicVector(&pd.CubicArrayY, c);
			}
			destroyHollowPtrVector(&HPV1);
			initHollowPtrVector(&HPV1);

			for(int i = 0; i < pd.dp->num; i++)
			{
				double screenX, screenY;
				graphToScreenCords(pd.dp->x.data[i],pd.dp->y.data[i],&screenX,&screenY,App->c);
				pushHollowPtrVector(&HPV1,createHollowPtr(screenX,screenY,15,App->renderer,NULL));
			}
			App->c->canvasRequiresReset = true;
			App->loadRequested = false;
		}
		if(App->saveRequested)
		{
			saveData(pd.dp, "data.txt");
			App->saveRequested = false;
		}
		if(App->c->canvasRequiresReset)
		{
			for(int i=0;i<pd.dp->num;i++) 
			{
				SDL_FRect r = HPV1.data[i]->b1->r;
				screenToGraphCords(r.x+r.w/2.0,r.y+r.h/2.0,&pd.dp->x.data[i],&pd.dp->y.data[i],App->c);
			}
			for(int i=0;i<pd.dp->num-1;i++)
			{
				double tempx = (pd.dp->x.data[i+1] - pd.dp->x.data[i]) *(pd.dp->x.data[i+1] - pd.dp->x.data[i]);
				double tempy = (pd.dp->y.data[i+1] - pd.dp->y.data[i]) *(pd.dp->y.data[i+1] - pd.dp->y.data[i]);
				pd.dp->h.data[i] = sqrt(tempx+tempy);
			}
			LinearSystem ls;
			initLinearSystem(&ls,pd.dp->num);
			fillLinearSystem(&ls,pd.dp);
			constructCubics(&ls,pd.dp,pd.CubicArrayX.data,pd.CubicArrayY.data);
			destroyLinearSystem(&ls);
			updateLabelFromDouble(drawTime,calcRunTime(App, drawCubics, &pd));
			App->c->canvasRequiresReset = false;
		}

		if(App->c->saveRequested) saveCurrentFrame(App->c);
		App->anyButtonPressed = false;

		for(int i=0;i<HPV1.size;i++) 
		{
			updateHollowPtr(&App->Mouse,HPV1.data[i],App->mousedown,App->anyButtonPressed);
			if(HPV1.data[i]->changeColour) App->anyButtonPressed = true;
		}

		if(App->anyButtonPressed) App->c->canvasRequiresReset = true;

		current_render_time = SDL_GetPerformanceCounter();
		App->elapsedNanoSeconds = (current_render_time - App->last_render_time)	* 1000000000ULL	/ App->freq;

		if(App->createPoint)
		{
			pushHollowPtrVector(&HPV1,createHollowPtr(App->Mouse.x,App->Mouse.y,15,App->renderer,NULL));
			double x,y;
			screenToGraphCords(App->Mouse.x, App->Mouse.x, &x, &y,App->c);
			addPlotDataPoint(&pd,x,y);
			App->createPoint = false;
			App->c->canvasRequiresReset = true;
		}
		if(App->bgimageRender && App->imgTexture==NULL)
		{
			App->imgTexture = IMG_LoadTexture(App->renderer, "img.png");
			if(!App->imgTexture) 
			{
				printf("img not found");
				App->running = false;
				return 1;
			}
		}
		if(App->elapsedNanoSeconds>=App->clockrate)
		{
			SDL_SetRenderDrawColor(App->renderer, 25, 25, 25, 255);
			SDL_RenderClear(App->renderer);
			if(!App->bgimageRender) SDL_RenderTexture(App->renderer,App->c->bgTexture,NULL,NULL);
			else SDL_RenderTexture(App->renderer,App->imgTexture,NULL,&borderRect);
			if(App->lineRender)SDL_RenderTexture(App->renderer,App->c->frameTexture,NULL,NULL);
			renderLabel(drawTime);
			renderLabel(l2);
			updateLabelFromDouble(fpscounterlabel,1e9 / App->elapsedNanoSeconds);
			updateLabelFromDouble(frametime,App->elapsedNanoSeconds * 1e-6);
			renderLabel(frametime);
			renderLabel(fpscounterlabel);
			if(App->pointRender) for(int i=0;i<HPV1.size;i++) renderHollowPtr(HPV1.data[i],App->renderer);
			SDL_SetRenderDrawColor(App->renderer,200,200,200,255);
			drawThickRectangle(App->renderer,borderRect,10);
			SDL_RenderPresent(App->renderer);
			App->last_render_time = current_render_time;
		}
	}
	destroyHollowPtrVector(&HPV1);
	destroyPlotData(&pd);
	AppQuit(App);
	TTF_Quit();
	SDL_Quit();
	return 0;
}