#ifndef DIALOG_H
#define DIALOG_H

#include "SDL_Surface.h"
#include "Sprite.h"
#include "SDL_Text.h"
#include "Rectangle.h"
#include "MabMisc.h"
#include "MaB_Types.h"
#include <string>

class Dialog
{
public:
	Rect rect; // contains pos and dimension in world coords

	static const int maxHL = 2; // maximum amount of lines to be rendered at a time
	int currentMsgHL;
	int currentOptionHL;

	Strs msgs;
	Strs msgHLs;
	Strs options;
	Strs optionHLs;

	bool dialogRead;

	Sprite moreButton;
	SDL_Surface* internal_surface;
	SDL_Surface *format_surface;	 
    uint32_t internal_value;         

	Text text;

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
		dialogRead = false;
		rect = Rect( 0, 0, width, height );
		format_surface = SDL_CreateRGBSurface( SDL_SWSURFACE, 1, 1, 32, 0, 0, 0, 0);
		internal_value = SDL_MapRGB(format_surface->format, 0, 0, 0);
		internal_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0);
		//SDL_SetColorKey(internal_surface, SDL_SRCCOLORKEY, internal_value);
		SDL_SetAlpha(internal_surface, SDL_SRCALPHA, 216);
		text.Init( 255, 255, 255, std::string( "consola.ttf" ), 14 );		
		
		moreButton.surfaces.push_back( Surface::PngLoad( "./art/dlg_more_arrow.png", true ) );
		moreButton.blinkingDelay = 444;
		// set initial values for rendering
		currentMsgHL = currentOptionHL = 0;
	}
	
	// calculate horizontal/vertical lines
	void CalcLines() 
	{ 
		CalcLines( msgs, msgHLs ); 
		CalcLines( options, optionHLs );
	}
	void CalcLines( const Strs& msgs, Strs& HLs ) 
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
			// Assuming every msg ends in a period or a space.
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
					// make sure we breaking our HLs off at spaces or periods.
					while ( 1 ) { 
						substr = msgs[i].substr( offs, (int)( r * (msgs[i].length() - offs) - n++ ) );
						if ( substr.back() == ' ' || substr.back() == '.')
							break;
					}

					w = text.TextSize( substr ).first;
				}
				
				HLs.push_back( substr );
				offs += substr.length();

				if ( offs < (int)msgs[i].length() )
					goto TOP;
			}
		}

		PostProcessHL( HLs );
	}
	void PostProcessHL( Strs& HLs )
	{
		// these HLs are with in the dialog boundary and are ready for 
		// post processing to make the text look nice. Inspired from Zelda. 
		// Rules for post processing:
		// 1) if a line only has one word and the line above is at least 75% 
		// the length of the dialog, move a word from the above line to  the 
		// line below.
		// 2) if a line has at least four spaces on it's sides, move the last 
		// word forward a spaces.

		int dw = (int)( rect.w * .9f );
		int twospaces = text.TextSize( std::string("  ") ).first;

		for ( int i=1; i < HLs.size(); ++i )
		{
			if ( MabMisc::CountWordsInString( HLs[i] ) == 1 )
			{
				// move word from above line to current line
				if ( text.TextSize( HLs[ i - 1] ).first / (float)dw > .75f )
				{
					if ( HLs[i - 1].back() == ' ') HLs[i - 1].pop_back(); // don't want spaces at the end of my HL.
					size_t pos = HLs[i - 1].rfind(' ');
					HLs[i].insert(0, HLs[i - 1].substr( pos + 1) + " " );
					HLs[i - 1].erase(pos + 1);
				}
			}
		}

		for ( std::string& s : HLs )
		{
			if ( dw - text.TextSize( s ).first > (twospaces*2) )
			{
				// shift last word forward
				if ( s.back() == ' ') 
					s.pop_back(); // don't want spaces at the end of my HL.
				size_t pos = s.rfind(' ');
				s.insert(pos, " ");
			}
		}
	}

	void Update()
	{
		moreButton.Update();
	}
	void Render( SDL_Surface* dest, const Rect& cr )
	{
		if ( dialogRead == true ) 
			return;

		UpdateRect( cr ); // position the dialog under the camera in world space

		// transform dialog from world space to camera space
		Rect r = rect.SubtractPosition( cr );
		
		// draw dialog background
		SDL_FillRect(internal_surface, NULL, internal_value );

		// draw horizontal lines of text
		int dw = (int)( r.w * .9f ); // take into account 10% for side margins

		// render msg hls          
		int y = 0;
		int linesRendered = 0;
		for ( int i = currentMsgHL; i < (int) msgHLs.size()  && 
			  linesRendered < maxHL; ++i, y += 14, linesRendered++ )
		{
			int w = text.TextSize( msgHLs[i] ).first;   
			int x = (int)r.w*.05f;//(int)( r.w*.05f + ( dw - w ) / 2 );
			text.Render( internal_surface, x, y, msgHLs[i] );
		}
		
		if ( linesRendered < maxHL ) 
		{
			for ( int i = currentOptionHL; i < (int) optionHLs.size()  && 
				  linesRendered < maxHL; ++i, y += 14, linesRendered++ )
			{
				int w = text.TextSize( optionHLs[i] ).first;   
				int x = (int)r.w*.05f;//(int)( r.w*.05f + ( dw - w ) / 2 );
				text.Render( internal_surface, x, y, optionHLs[i] );
			}			
		}

		// render "more text" button if there's more text to be displayed
		if ( currentMsgHL < (int)( msgHLs.size() - maxHL ) ||
			 currentOptionHL < (int)( optionHLs.size() - maxHL ) )
		{
			moreButton.Render(internal_surface, ( r.w - moreButton.Width() ) / 2, 
							 internal_surface->h - moreButton.Height() );
		}
		Surface::OnDraw( dest, internal_surface, r.left, r.top );

	}

	void LButtonDown(int mX, int mY)  
	{
		if ( currentMsgHL < (int)( msgHLs.size() ) ) 
		{
			if ( optionHLs.size() == 0 && currentMsgHL == (int)( msgHLs.size() - maxHL ) )
				dialogRead = true;

			currentMsgHL++;
			return; 
		}

		if ( currentOptionHL < (int)( optionHLs.size() - maxHL ) ) {
			currentOptionHL++;
			return;
		}

		if ( currentOptionHL == (int)( optionHLs.size() - maxHL ) )
			dialogRead = true;
		
	} 
};

#endif