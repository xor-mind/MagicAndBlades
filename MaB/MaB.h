#ifndef MAB_H
#define MAB_H

#include "2DMath.h"
#include "SDL_app.h"
#include "types.h"
#include "winapi_timer.h"
#include <string>
#include <vector>
#include <iostream>
#include "entity.h"
#include <boost/assign/std/vector.hpp> // for 'operator+=()'
using namespace boost::assign; // bring 'operator+=()' into scope

class Map
{
protected:
	uint w,h; 
public:

	class Cell
	{ 
	public:
	};

	std::vector<SDL_Surface * > data;
};

class HomeLand : public Map
{
private:
	SDL_Surface * grass, * dirt;
	Player player;
	Entity camera;

public:
	HomeLand() { w = h = 80; }

	void Init(Vector screenDim)
	{
		camera.dim = Vector( 32*16, 32*8 );
		camera.pos = camera.vel = Vector( 0, 0 );
		grass = Surface::BmpLoad( "./art/grass01.bmp" );
		dirt  = Surface::BmpLoad( "./art/grass02.bmp" );

		for ( int y = 0; y < (int)h; ++y )
			for ( int x = 0; x < (int)w; ++x )
			{
				if ( y % 2 != 0 )	
					data.push_back( x % 2 != 0 ? dirt : grass ); 
				else 
					data.push_back( x % 2 != 0 ? grass : dirt );
			}
		player.Init();
	}
	void Logic()
	{
		player.Logic();
		//static auto startTime = GetTickCount();
		//if ( GetTickCount() - startTime > 150 )
		{
			camera.pos += camera.vel;
			Bound(camera);
			Bound(player);
			//startTime = GetTickCount();
		}
	}
	// restricts an entities position to within the map
	void Bound(Entity& e)
	{
		e.pos.x = max( 0, min( e.pos.x, w*32 - e.dim.x ) );
		e.pos.y = max( 0, min( e.pos.y, h*32 - e.dim.y ) );
	}
	void Render( SDL_Surface * display )
	{
		auto  rCamDim   = camera.dim, // remaining camera dimension
		      cCamPos   = camera.pos, // current camera pos 
			  screenPos = Vector(0, 0);

		// render map
		while ( rCamDim.y > 0 )
		{
			// tile to render
			int tile = (int)std::floor( cCamPos.x / 32 ) 
				+ (int)std::floor( cCamPos.y / 32 ) * w;

			// local coords of tile to render
			auto tilePos = Vector( (float)((int)cCamPos.x % 32), (float)((int)cCamPos.y % 32) ),
				 tileDim = Vector( min( 32 - tilePos.x, rCamDim.x ), 
								   min( 32 - tilePos.y, rCamDim.y ) );

			Surface::OnDraw( display, data[tile], (int)screenPos.x, (int)screenPos.y, (int)tilePos.x, (int)tilePos.y, (int)tileDim.x, (int)tileDim.y );                                                            

			screenPos.x += tileDim.x;
			cCamPos.x   += tileDim.x;
			rCamDim.x   -= tileDim.x;

			if ( rCamDim.x <= 0)
			{
				screenPos.x = 0;
				screenPos.y += tileDim.y;
				cCamPos.y   += tileDim.y;
				cCamPos.x   = camera.pos.x;
				rCamDim.x   = camera.dim.x;
				rCamDim.y   -= tileDim.y;
			}
		}

		// render entities
		if ( camera.Rect().Intersect( player.Rect() ) )
		{
			math::Rectangle c( camera.pos, camera.dim ), // c = camera rect
						    e( player.pos, player.dim ); // e = entity rect
			// rect on rect clipping using deltas to capture the region showing in the cam.
			int dcReL = c.right  - e.left,
				deRcL = e.right  - c.left,
				dcBeT = c.bottom - e.top,
				deBcT = e.bottom - c.top;
			int l = max( c.left, c.right - dcReL ),
				r = min( c.right, c.left + deRcL ),
				t = max( c.top, c.bottom - dcBeT ),
				b = min( c.bottom, c.top + deBcT );
			// translate to screen coords
			int x = (int)(l - c.left), 
				y = (int)(t - c.top),
				srcX = (int)(l - e.left),
				srcY = (int)(t - e.top),
				srcW = (int)( r - l ),
				srcH = (int)( b - t );
			Surface::OnDraw( display, player.model, x, y, srcX, srcY, srcW, srcH );
		}
		//while ( rCamDim.x > 0 )
		//{
		//	// check for intersection between camera and entity
		//	Rectangle camera
		//}

		//player.Render( display );
	}
	void KeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
	{
		player.KeyDown( sym, mod, unicode );

		switch ( sym )
		{
			case SDLK_RIGHT: camera.vel.x = 4; break;
			case SDLK_LEFT: camera.vel.x = -4; break;
			case SDLK_UP: camera.vel.y = -4; break;
			case SDLK_DOWN: camera.vel.y = 4; break;
				default: break;
		}
	}	
	void KeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
	{
		player.KeyUp( sym, mod, unicode );
		switch ( sym )
		{
			case SDLK_RIGHT: camera.vel.x = 0; break;
			case SDLK_LEFT: camera.vel.x = 0; break;
			case SDLK_UP: camera.vel.y = 0; break;
			case SDLK_DOWN: camera.vel.y = 0; break;
				default: break;
		}
	}
};

class MaB : public SDL_App
{
	HomeLand hl;
	SDL_Surface * display;
	HighPerformanceTimer hpt;
	Vector screenDim;
public:
	MaB() {}
	~MaB() {}
	
	void KeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) override
	{
		hl.KeyDown( sym, mod, unicode );
	}
	void KeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) override
	{
		hl.KeyUp( sym, mod, unicode );
	}
	bool Init() override
	{
		screenDim = Vector( 600, 600 );
		if((display = SDL_SetVideoMode( (int)screenDim.x, (int)screenDim.y, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) 
		{
			return false;
		}
		hl.Init( screenDim );

		hpt.InitTimer(20); 
		// _putenv("SDL_VIDEODRIVER=windib"); whats frame rate with this in?
		return true;
	}
	void Render() override
	{
		hl.Render( display );
		SDL_Flip(display);
	}
	void Logic() override 
	{ 
		static bool once=true; if ( once ) { hpt.UpdateTime0(); once = false; }
		//static auto logicFPS = 0;
		//static auto startTime = GetTickCount();
		hpt.UpdateTime1();
		if ( hpt.DeltaTime() >= hpt.LogicTime() )
		{
			hl.Logic(); 
			//logicFPS++;
			hpt.UpdateFixedStep();
		}

		//if ( GetTickCount() - startTime >= 1000 )
		//{
		//	std::cout << logicFPS << std::endl; logicFPS=0;
		//	startTime = GetTickCount();
		//}
	}
	
};

class Server
{
	
};

/*void gameFlow()
{
	Quest.title = "The lumberjack's Curse".
	Quest.Summary = "Long ago there was a lumberjack named Terry. Terry was a curious lad and he was deep in the forest looking for unusual wood. The rarer the wood, the more valuable his pay. He a quaint clearing he saw a felled tree that seemed to sparkle. Thinking this was his lucky day he rushed into the clear with lust sparkling in his terry's eyes. As he approached the tree a Giant daemon phased into existent, sitting on the tree. The deamon stared deeped into Terry's eyes, into his soul. The deamon's eyes where a green yellow, and appeared to be alive with fire. Terry felt drawn into the gaze, and he could see deeper and deeper into the realm behind the daemon. \"Terry\" said, the deamon, \"You've come to a forbidden place, this tree was my loved one died many eons ago in a ferocious battle with the natives of this land. I mourn for here.";

	Deamon.Spawn();
}*/

#endif