#ifndef WIDGETS_H
#define WIDGETS_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "app.h"

//conaider changing all layout rects to centre or top left?? for uniformity

struct button
{
	SDL_FRect r;//topleft	
	bool currentState;
	bool prevState;
	bool clickedThisFrame;
	bool releasedThisFrame;
	bool mouseHovering; //maybe combine to do bitmasking
	/*
	improve button collison behaviour
	when mousedown and moves out of button, it no longer registers
	when mousedown and moves into button, it registers
	*/
};
typedef struct button* Button;

struct label
{
	SDL_FPoint pos;//topleft
	TTF_Text* text;
};
typedef struct label* Label;

struct slider
{
	Button b;
	bool changed;
	float val; //-1.0 to 1.0
	SDL_FRect r;//topleft
	Label lLabel;
	Label rLabel;
};
typedef struct slider* Slider;

struct hollowptr
{
	Button b1;
	SDL_Texture* texture;
	bool changeColour;
};
typedef struct hollowptr* HollowPtr;

typedef struct
{
	HollowPtr* data;
	int size;
	int capacity;
}HollowPtrVector;

Button createButton(float x, float y, float w, float h); //x,y is center
void renderButton(Button, SDL_Renderer*);
void updateButtonState(SDL_FPoint* ,Button,bool mouseState);
bool isButtonCurrentlyPressed(Button);
bool wasButtonClicked(Button);
bool wasButtonReleased(Button);
void destroyButton(Button);
void renderSliderButton(Button,SDL_Renderer*);

Slider createSlider(float x, float y, float w, float h,struct textsys TextSys); //x,y is center
void renderSlider(Slider, SDL_Renderer*);
void updateSliderState(SDL_FPoint* ,Slider,bool mouseState);
void destroySlider(Slider);

Label createLabel(float x, float y, struct textsys TextSys, char* textlabel); //x,y is topleft
void renderLabel(Label);
void updateLabelText(Label l1, char* textlabel);
void updateLabelFromDouble(Label l1, double x);
void destroyLabel(Label);

HollowPtr createHollowPtr(float x, float y, float r, SDL_Renderer*,SDL_Texture*);
void destroyHollowPtr(HollowPtr);
void renderHollowPtr(HollowPtr, SDL_Renderer*); //first mouseclick inconsistent
void updateHollowPtr(SDL_FPoint* ,HollowPtr,bool mouseState,bool somethingElsePressed);

//the following are for array versions

void initLabels(Label * lArr,int num, Application a1);
void destroyLabels(Label * lArr,int num);
void renderLabels(Label * lArr,int num);
void updateLabels(Label * lArr,int num,float* values);
void changeRlabels(Slider* sArr,int num);

void initButtons(Button * bArr,int num, Application a1);
void destroyButtons(Button * bArr,int num);
void renderButtons(Button * bArr,int num,SDL_Renderer*);
void updateButtonStates(SDL_FPoint* ,Button * bArr,int num,bool mouseState);
void updateButtonValues(Button* bArr,int num,float *values);

void initSliders(Slider * sArr,int num, Application a1);
void destroySliders(Slider * sArr,int num);
void renderSliders(Slider * sArr,int num, SDL_Renderer*);
void updateSliders(SDL_FPoint* ,Slider* sArr,bool mouseState,int num);

void initHollowPtrVector(HollowPtrVector*);
void pushHollowPtrVector(HollowPtrVector*, HollowPtr);
void deleteHollowPtrVector(HollowPtrVector*, int index);
void destroyHollowPtrVector(HollowPtrVector*);

#endif