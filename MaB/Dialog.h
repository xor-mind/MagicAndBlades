#ifndef DIALOG_H
#define DIALOG_H

#include "SDL_Surface.h"
#include "Sprite.h"
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

	Sprite moreButton;
	SDL_Surface* internal_surface;
	SDL_Surface *format_surface;	 
    uint32_t internal_value;         

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
	virtual ~Dialog()
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

	void Init()
	{
		rect = Rect( 0, 0, width, height );
		format_surface = SDL_CreateRGBSurface( SDL_SWSURFACE, 1, 1, 32, 0, 0, 0, 0);
		internal_value = SDL_MapRGB(format_surface->format, 0, 0, 0);
		internal_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0);
		//SDL_SetColorKey(internal_surface, SDL_SRCCOLORKEY, internal_value);
		SDL_SetAlpha(internal_surface, SDL_SRCALPHA, 216);
		text.Init( 255, 255, 255, std::string( "consola.ttf" ), 14 );		
		
		moreButton.surfaces.push_back( Surface::PngLoad( "./art/dlg_more_arrow.png", true ) );
		moreButton.blinkingDelay = 444;
	}
	
	// calculate horizontal/vertical lines
	void CalcLines() { CalcLines( msgs ); }
	void CalcLines( Strs msgs ) 
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

		// set initial values for rendering
		currentHL = 0;
	}
	static const int maxHL = 2; // maximum amount of lines to be rendered at a time
	int currentHL;

	void Update()
	{
		moreButton.Update();
	}
	void Render( SDL_Surface* dest, const Rect& cr )
	{
		// transform dialog from world space to camera space
		Rect r = rect.SubtractPosition( cr );
		
		// draw dialog background
		SDL_FillRect(internal_surface, NULL, internal_value );

		// draw horizontal lines of text
		int dw = (int)( r.w * .9f ); // take into account 10% for side margins

		// render text            
		int y = 0;
		for ( int i = currentHL; i < (int)HLs.size() && i < (currentHL + maxHL);
			++i, y += 14 )
		{
			int w = text.TextSize( HLs[i] ).first;   
			int x = (int)( r.w*.05f + ( dw - w ) / 2 );
			text.Render( internal_surface, x, y, HLs[i] );
		}

		// render "more text" button if there's more text to be displayed
		if ( currentHL != maxHL )
		{
			moreButton.Render(internal_surface, ( r.w - moreButton.Width() ) / 2, 
							 internal_surface->h - moreButton.Height() );
		}
		Surface::OnDraw( dest, internal_surface, r.left, r.top );

	}
};

class InteractiveDialog : public Dialog
{
public:
	Strs options;
public:
	~InteractiveDialog() {}
};

#endif