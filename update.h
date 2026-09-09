#ifndef UPDATE_H
#define UPDATE_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include "canvas.h"
#include "widgets.h"
#include "app.h"

void poller(SDL_Event,Application);
int dtod(int xord, int ycord, Canvas);
double getRadius(double,double);
void handlefulls(SDL_Window*,bool*); //causes a LOOTT of issues
void saveCurrentFrame(Canvas);
void keydowncontrols(SDL_Event event,Application App);
void saveData(DataPoints* dp, const char* filename);
void loadData(DataPoints* dp, const char* filename);

#endif