#include "widgets.h"
#include "app.h"
#include "render.h"
#include "stdlib.h"
#include "update.h"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>
#include "app.h"
#include <stdio.h>

Button createButton(float x, float y, float w, float h)
{
	Button b1 = (Button) malloc(sizeof(struct button));
	b1->r = (SDL_FRect){x-w/2,y-h/2,w,h};
	b1->currentState = false;
	b1->prevState = false;
	b1->clickedThisFrame = false;
	b1->releasedThisFrame = false;
	b1->mouseHovering = false;
	return b1;
}

void renderButton(Button b1, SDL_Renderer* renderer)
{
	if(b1->currentState) SDL_SetRenderDrawColor(renderer, 143, 143, 114, 255);
	else if(b1->mouseHovering) SDL_SetRenderDrawColor(renderer, 150, 150, 138, 255);
	else SDL_SetRenderDrawColor(renderer, 192, 193, 194, 255); //unpressed,normal state
	SDL_RenderFillRect(renderer,&b1->r);
}

void updateButtonState(SDL_FPoint* Mouse, Button b1, bool mouseState)
{
	b1->prevState = b1->currentState;
	bool colliding = SDL_PointInRectFloat(Mouse,&b1->r);
	b1->mouseHovering = !mouseState && colliding;
	b1->clickedThisFrame = !b1->prevState && colliding && mouseState;
	if(b1->clickedThisFrame) b1->currentState = true;
	b1->releasedThisFrame = b1->prevState && !mouseState;
	if(b1->releasedThisFrame) b1->currentState = false;
}

bool isButtonCurrentlyPressed(Button b1)
{
	return b1->currentState;
}

bool wasButtonClicked(Button b1)
{
	return b1->clickedThisFrame;
}
bool wasButtonReleased(Button b1)
{
	return b1->releasedThisFrame;
}

void destroyButton(Button b1)
{
	free(b1);
}



Slider createSlider(float x, float y, float w, float h,struct textsys TextSys)
{
	Slider s1 = (Slider) malloc(sizeof(struct slider));
	s1->r = (SDL_FRect) {x-w/2,y-h/2,w,h};
	s1->val = 0;
	s1->b = createButton(x,y,w/10,3*h);
	s1->changed = true;
	s1->rLabel = createLabel(x+w/2.0, y-h,TextSys,"0");
	s1->lLabel = createLabel(x-w/2.0-90, y-h,TextSys,"0");
	return s1;
}

void renderSlider(Slider s1, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 192, 193, 194, 255); //unpressed,normal state
	SDL_RenderFillRect(renderer,&s1->r);
	renderSliderButton(s1->b, renderer);
	renderLabel(s1->rLabel);
	renderLabel(s1->lLabel);
}

void renderSliderButton(Button b1,SDL_Renderer* renderer)
{
	if(b1->currentState) SDL_SetRenderDrawColor(renderer, 142, 143, 144, 230);
	else if(b1->mouseHovering) SDL_SetRenderDrawColor(renderer, 152, 153, 154, 230);
	else SDL_SetRenderDrawColor(renderer, 192, 193, 194, 230); //unpressed,normal state
	SDL_RenderFillRect(renderer,&b1->r);
}

void updateSliderState(SDL_FPoint* mouse, Slider s1, bool mouseState)
{
	Button sliderButton = s1->b;
	updateButtonState(mouse, sliderButton, mouseState);
	float buttonwidth = sliderButton->r.w;
	float currx = mouse->x - s1->r.x;
	float sliderwidth = s1->r.w;
	if(isButtonCurrentlyPressed(sliderButton))
	{
		if(currx > buttonwidth/2 && currx < sliderwidth - buttonwidth/2.0)
		{
			sliderButton->r.x = mouse->x - buttonwidth/2.0;
			s1->val = -10.0 + 20.0 * (currx - buttonwidth/2.0) / (sliderwidth-buttonwidth);
			s1->changed = true;
		}
	}
	updateLabelFromDouble(s1->rLabel,s1->val); 
}

void destroySlider(Slider s1)
{
	destroyLabel(s1->rLabel);
	destroyLabel(s1->lLabel);
	destroyButton(s1->b);
	free(s1);
}



Label createLabel(float x, float y, struct textsys TextSys, char* textlabel)
{
	Label l1 = (Label) malloc(sizeof(struct label));
	l1->pos = (SDL_FPoint) {x,y};
	l1->text = TTF_CreateText(TextSys.textengine,TextSys.font,textlabel, strlen(textlabel));
	TTF_SetTextColor(l1->text, 255, 255, 255, 255);
	return l1;
}

void renderLabel(Label l1)
{
	TTF_DrawRendererText(l1->text, l1->pos.x, l1->pos.y);
}

void updateLabelText(Label l1, char* textlabel)
{
	TTF_SetTextString(l1->text, textlabel, strlen(textlabel));
}

void updateLabelFromDouble(Label l1, double x)
{
	char buff[255];
	snprintf(buff, sizeof(buff), "%0.2lf",x);
	TTF_SetTextString(l1->text, buff, strlen(buff));
}

void destroyLabel(Label l1)
{
	TTF_DestroyText(l1->text);
	free(l1);
}



HollowPtr createHollowPtr(float x, float y, float r, SDL_Renderer* renderer,SDL_Texture* t)
{
	HollowPtr p1 = (HollowPtr) malloc(sizeof(struct hollowptr));
	p1->b1 = createButton(x, y, 2*r, 2*r);
	p1->texture =createHollowPtrTexture(r, renderer);
	p1->changeColour = false;
	return p1;
}

void destroyHollowPtr(HollowPtr p1)
{
	SDL_DestroyTexture(p1->texture);
	destroyButton(p1->b1);
	free(p1);
}

void renderHollowPtr(HollowPtr p1, SDL_Renderer * renderer)
{
	if(p1->changeColour)
	{
		if(p1->b1->currentState) 
		{
			SDL_SetTextureColorModFloat(p1->texture, 0.8438, 0.844, 0.845);
			SDL_RenderTexture(renderer, p1->texture, NULL, &p1->b1->r);
			SDL_SetTextureColorModFloat(p1->texture, 1.185, 1.1849, 1.1835);
		}
	}
	else SDL_RenderTexture(renderer, p1->texture, NULL, &p1->b1->r);
}

void updateHollowPtr(SDL_FPoint * mouse, HollowPtr p1, bool mouseState, bool somethingElsePressed)
{
	if(!p1->b1->currentState && somethingElsePressed) return;
	updateButtonState(mouse, p1->b1, mouseState);
	if(p1->b1->currentState /*|| p1->b1->mouseHovering*/) p1->changeColour = true;
	else p1->changeColour = false;
	if(isButtonCurrentlyPressed(p1->b1))
	{
		p1->b1->r.x = mouse->x - p1->b1->r.w/2.0;
		p1->b1->r.y = mouse->y - p1->b1->r.h/2.0;
	}
}

void initLabels(Label * lArr,int num, Application a1)
{
	char buf[8];
	for(int i =0; i<num;i++)
	{
		sprintf(buf, "%d", i+1);
		lArr[i]=createLabel(a1->c->width - 400.0,50*i + 100, a1->TextSys, buf);
	}
}

void destroyLabels(Label * lArr,int num)
{
	for(int i =0; i<num;i++)
	{
		destroyLabel(lArr[i]);
	}
}

void renderLabels(Label * lArr,int num)
{
	for(int i =0; i<num;i++)
	{
		renderLabel(lArr[i]);
	}
}

void updateLabels(Label * lArr,int num,float* values)
{
	for(int i =0; i<num;i++)
	{
		updateLabelFromDouble(lArr[i], values[i]);
	}
}

void initButtons(Button * bArr,int num, Application a1)
{
	for(int i =0; i<num;i+=2)
	{
		bArr[i]=createButton(a1->c->width - 200.0,25*i + 130,50,30);
		bArr[i+1]=createButton(a1->c->width - 130.0,25*i + 130,50,30);
	}
}

void destroyButtons(Button * bArr,int num)
{
	for(int i =0; i<num;i++)
	{
		destroyButton(bArr[i]);
	}
}

void renderButtons(Button * bArr,int num,SDL_Renderer* renderer)
{
	for(int i =0; i<num;i++)
	{
		renderButton(bArr[i],renderer);
	}
}

void updateButtonStates(SDL_FPoint* mouse,Button * bArr,int num,bool mouseState)
{
	for(int i =0; i<num;i++)
	{
		updateButtonState(mouse, bArr[i], mouseState);
	}
}

void updateButtonValues(Button* bArr,int num,float *values)
{
	for(int i =0; i<num;i++)
	{
		Button b1 = bArr[i];
		if(b1->releasedThisFrame && i%2==0 && values[i/2]>-0.95) values[i/2]-=0.05;
		else if(b1->releasedThisFrame && i%2!=0 && values[i/2]<0.95) values[i/2]+=0.05;
	}
}

void initSliders(Slider * sArr,int num, Application a1)
{
	for(int i =0; i<num;i++)
	{
		int x = i/4;
		sArr[i]=createSlider(a1->c->width - 200.0,100*i +50+ 30*x,200,20,a1->TextSys);
	}
}

void destroySliders(Slider * sArr,int num)
{
	for(int i =0; i<num;i++)
	{
		destroySlider(sArr[i]);
	}
}

void renderSliders(Slider * sArr,int num, SDL_Renderer* renderer)
{
	for(int i =0; i<num;i++)
	{
		renderSlider(sArr[i],renderer);
	}
}

void updateSliders(SDL_FPoint* mouse,Slider* sArr,bool mouseState,int num)
{
	for(int i =0; i<num;i++)
	{
		updateSliderState(mouse,sArr[i],mouseState);
	}
}

void changeRlabels(Slider* sArr,int num)
{
	for (int i = 0; i < num; i++)
	{
		char buff[20];
		int c = 1 + i%4;
		int r = 1 + i/4;
		snprintf(buff,sizeof(buff),"w%d%d",r,c);
		updateLabelText(sArr[i]->lLabel,buff);
	}
}

void initHollowPtrVector(HollowPtrVector* HPV1)
{
	HPV1->capacity = 10;
	HPV1->size = 0;
	HPV1->data = (HollowPtr*) malloc(10*sizeof(HollowPtr));
}

void pushHollowPtrVector(HollowPtrVector* HPV1, HollowPtr hp1)
{
	if(HPV1->size==HPV1->capacity) 
	{
		HPV1->capacity *= 2;
		HPV1->data = realloc(HPV1->data,HPV1->capacity * sizeof(HollowPtr));
	}
	HPV1->data[HPV1->size] = hp1;
    HPV1->size++;
}

void destroyHollowPtrVector(HollowPtrVector* HPV1)
{
	for(int i=0;i<HPV1->size;i++) destroyHollowPtr(HPV1->data[i]);
}