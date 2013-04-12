#ifndef SDL_SURFACE_H
#define SDL_SURFACE_H

#include <SDL.h>
#include <SDL_image.h>
#include <functional>

class Surface 
{
    public:
		Surface() {}
 
    public:
        static SDL_Surface* BmpLoad(char* File)
		{
			SDL_Surface* Surf_Temp = NULL;
			SDL_Surface* Surf_Return = NULL;
 
			if((Surf_Temp = SDL_LoadBMP(File)) == NULL) {
				return NULL;
			}
 
			Surf_Return = SDL_DisplayFormat(Surf_Temp);
			SDL_FreeSurface(Surf_Temp);
 
			return Surf_Return;
		}

		static SDL_Surface* PngLoad(char* File)
		{
			SDL_Surface* Surf_Temp = NULL;
			SDL_Surface* Surf_Return = NULL;
 
			if((Surf_Temp = IMG_Load(File)) == NULL) {
				return NULL;
			}
 
			Surf_Return = SDL_DisplayFormat(Surf_Temp);
			SDL_FreeSurface(Surf_Temp);
 
			return Surf_Return;
		}

		static bool OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y) 
		{
			if(Surf_Dest == NULL || Surf_Src == NULL) {
				return false;
			}
 
			SDL_Rect DestR;
 
			DestR.x = X;
			DestR.y = Y;
 
			SDL_BlitSurface(Surf_Src, NULL, Surf_Dest, &DestR);
 
			return true;
		}

		static bool OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y, int X2, int Y2, int W, int H)
		{
			if(Surf_Dest == NULL || Surf_Src == NULL) {
				return false;
			}
 
			SDL_Rect DestR;
 
			DestR.x = X;
			DestR.y = Y;
 
			SDL_Rect SrcR;
 
			SrcR.x = X2;
			SrcR.y = Y2;
			SrcR.w = W;
			SrcR.h = H;
 
			SDL_BlitSurface(Surf_Src, &SrcR, Surf_Dest, &DestR);
 
			return true;
		}

		static bool Transparent(SDL_Surface* Surf_Dest, int R, int G, int B) 
		{
			if( Surf_Dest == NULL) {
				return false;
			}
 
			SDL_SetColorKey(Surf_Dest, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(Surf_Dest->format, R, G, B));
 
			return true;
		}
		static bool PinkTransparent(SDL_Surface* Surf_Dest) {
			return Transparent(Surf_Dest, 255, 0, 255);
		}
};

#endif 