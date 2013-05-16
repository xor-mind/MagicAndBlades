#ifndef DIALOG_H
#define DIALOG_H

#include "SDL_Surface.h"
#include "SDL_Text.h"
#include "Rectangle.h"
#include "MaB_Types.h"
#include <string>


class Dialog
{
public:
	Rect rect;
	Strs msgs;
	Text text;

	SDL_Surface* s;
	SDL_Surface* internal_surface;
	SDL_Surface *format_surface;	  /**< a dummy surface used to get a pixel format */
    uint32_t internal_value;         /**< the SDL color encapsulated */

public:
	~Dialog()
	{
		SDL_FreeSurface(internal_surface);
		SDL_FreeSurface(format_surface);
	}

	void Init()
	{
		rect.left = rect.top = 10;
		rect.w = rect.h = 200;
		format_surface = SDL_CreateRGBSurface( SDL_SWSURFACE, 1, 1, 32, 0, 0, 0, 0);
		internal_value = SDL_MapRGB(format_surface->format, 0, 0, 0);
		internal_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 200, 200, 32, 0, 0, 0, 0);
		//SDL_SetColorKey(internal_surface, SDL_SRCCOLORKEY, internal_value);
		SDL_SetAlpha(internal_surface, SDL_SRCALPHA, 216);
		
		text.Init( 255, 255, 255, std::string( "consola.ttf" ), 14 );
		msgs.push_back( Str( "Hello World" ) );
		msgs.push_back( Str( "Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat." ) );
	}

	void Render( SDL_Surface* dest )
	{
		// render background box
		SDL_FillRect(internal_surface, NULL, internal_value );
		Surface::OnDraw( dest, internal_surface, 10, 10 );

		// render text
		            
		int dw = rect.w * .9f;
		Strs HLs;

		// figure out how many vertical lines(VL) by:
		// 	breaking my string(s) into horizontal lines(HL)
	
		// Breaking strings into HLs:
		for ( int i = 0; i < msgs.size(); ++i )
		{
			int offs = 0;
TOP:
			//Str output = msgs[i].substr( offs, std::string::npos );
			int w = text.TextSize( msgs[i].substr( offs, std::string::npos ) ).first;

			if ( w <= dw )
			{	
				HLs.push_back( msgs[i].substr( offs, std::string::npos ) );
			}
			else // w > dw
			{
				float o = w / (float) dw; // what percentage are we overshooting(o) dialog width?
				float r = 1.f/o;  // the reciprocal(r) should be size to make the input width the same as the dw
				
				Str substr;
				
				// keep subtracting characters until we're inside the dlg boundary
				int n = 0;
				while ( w > dw )
				{
					substr = msgs[i].substr(offs, r * (msgs[i].length() - offs) - n++);
					w = text.TextSize( substr ).first;
				}
				HLs.push_back( substr );
				offs += substr.length();

				if ( offs < msgs[i].length() )
					goto TOP;
			}
		}

		int y = rect.top;
		for ( int i = 0; i < HLs.size(); ++i, y += 14 )
		{
			int w = text.TextSize( HLs[i] ).first;   
			int x = rect.left + ( dw - w ) / 2;
			text.Render( dest, x, y, HLs[i] );
		}
	}

	
};

#endif