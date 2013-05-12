// various rendering routines
#ifndef VIDEO_H
#define VIDEO_H

// ad_todo: move this file to my sdlOOP library
#include "SDL.h"

void setPixel(int x,int y,int r,int g,int b);
void verticalLine(int y1,int y2,int x);
void horizontalLine(int x1,int x2,int y);
void renderPerimiter(SDL_Rect* rec);

#endif