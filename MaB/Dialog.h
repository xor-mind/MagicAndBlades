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
	Strs HLs;

	SDL_Surface* internal_surface;
	SDL_Surface *format_surface;	  /**< a dummy surface used to get a pixel format */
    uint32_t internal_value;         /**< the SDL color encapsulated */

	static const int width = 340, height = 100;
public:
	Dialog()
	{
		Init();
	}
	Dialog( const Rect& cam )
	{
		Init();
	    UpdateRect(cam );
	}
	~Dialog()
	{
		SDL_FreeSurface(internal_surface);
		SDL_FreeSurface(format_surface);
	}

	void UpdateRect( const Rect& cam )
	{
		int x = (int)( cam.left + ( cam.w - width ) / 2 ), 
			y = (int)( cam.bottom - height - .07f * cam.h ); 
		rect = Rect( x, y, x + width, y + height );
	}

	void Init(  )
	{
		//pdateRect( cam );
		rect = Rect( 0, 0, width, height );
		format_surface = SDL_CreateRGBSurface( SDL_SWSURFACE, 1, 1, 32, 0, 0, 0, 0);
		internal_value = SDL_MapRGB(format_surface->format, 0, 0, 0);
		internal_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0);
		//SDL_SetColorKey(internal_surface, SDL_SRCCOLORKEY, internal_value);
		SDL_SetAlpha(internal_surface, SDL_SRCALPHA, 216);
		text.Init( 255, 255, 255, std::string( "consola.ttf" ), 14 );		
		
		//msgs.push_back( Str( "Hello World" ) );
		//msgs.push_back( Str( "There was once a cat named Tiffany and Tiffany got struck by lightning! It really fucking sucked. I hope everything works out for Tiffany in the end. She's a good girl..." ) );
		//msgs.push_back( Str( "V vvv VvVv Vv" ) );

	}

	// calculate horizontal/vertical lines
	void CalcLines() 
	{
		Rect r = rect;
		
		// dialog width will be width of dialog rest with 10% for the side margins
		int dw = (int)( r.w * .9f );
		
		// figure out how many vertical lines(VL) by:
		// 	breaking my string(s) into horizontal lines(HL)
	
		// Breaking strings into HLs:
		for ( int i = 0; i < (int)msgs.size(); ++i )
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
					substr = msgs[i].substr( offs, (int)( r * (msgs[i].length() - offs) - n++ ) );
					w = text.TextSize( substr ).first;
				}
				HLs.push_back( substr );
				offs += substr.length();

				if ( offs < (int)msgs[i].length() )
					goto TOP;
			}
		}
	}

	void Render( SDL_Surface* dest, const Rect& cr )
	{
		//if ( !msgs.size() ) 
		//	return;
		// render background box
		Rect r = rect.SubtractPosition( cr );
		SDL_FillRect(internal_surface, NULL, internal_value );
		Surface::OnDraw( dest, internal_surface, r.left, r.top );
		int dw = (int)( r.w * .9f );
		// render text
		            
		int y = r.top;
		for ( int i = 0; i < (int)HLs.size(); ++i, y += 14 )
		{
			int w = text.TextSize( HLs[i] ).first;   
			int x = (int)( r.left + r.w*.05f + ( dw - w ) / 2 );
			text.Render( dest, x, y, HLs[i] );
		}
	}

	
};

#endif