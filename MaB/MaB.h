#ifndef MAB_H
#define MAB_H

#include "SDL_app.h"
#include "types.h"
#include "winapi_timer.h"
#include <string>
#include <vector>
#include <iostream>
#include "entity.h"
#include <boost/assign/std/vector.hpp> // for 'operator+=()'
#include "SDL_FpsCounter.h"
#include "SDL_Text.h"
#include <algorithm>
#include "SDL_Gfx.h"
#include "Dialog.h"
#include "Game.h"
#include <memory>
using namespace boost::assign; // bring 'operator+=()' into scope


class Map
{
protected:
	uint w,h; 
public:
	virtual ~Map() {}
	class Cell
	{ 
	public:
	};

	std::vector<SDL_Surface * > data;
};

class HomeLand : public Map
{
private:
	SDL_Surface * grass, * dirt, * avatar;
	std::unique_ptr<Player> player;
	std::unique_ptr<AggGrool> aggGrool;
	std::unique_ptr<Entity> camera;
	EntityVector entities;
	SDL_Rect clipRect;
	SDL_Video video;
	Game& g;
	
public:
	HomeLand( Game& game) : g( game )
	{ 
		w = h = 80; 
		camera.reset( new Entity( g ) );
		player.reset( new Player( g ) );
		aggGrool.reset( new AggGrool( g ) );
	}

	void Init( SDL_Surface * display )
	{
		
		video.screen = display;
		camera->pos = camera->vel = Vector( 0, 0 );
		camera->dim = Vector( 32*16, 32*8 ); // map is 512x256

		clipRect.x = 0; clipRect.y =  0, clipRect.w = (Uint16)camera->dim.x; clipRect.h = (Uint16)camera->dim.y;
		video.clipRect = clipRect;
		SDL_SetClipRect(display, &clipRect);

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
		avatar = Surface::BmpLoad("./art/avatar01.bmp");
		player->Init(avatar);
		aggGrool->Init(avatar);
		aggGrool->pos = Vector(32*8,32*8);
		entities.push_back( camera.get() );
		entities.push_back(( (Entity*)player.get() ));
		entities.push_back(( (Entity*)aggGrool.get() ));

		for( Entity* e: entities )
		{
			e->video = &video;
		}
	}
	void Logic()
	{
		player->Logic();
		aggGrool->Logic();

		camera->pos += camera->vel;
		for(Entity* e : entities)
		{
			Bound(*e);
		}

		if ( aggGrool->FoV.Intersect(player->Rectangle()) )
		{
			aggGrool->PlayerEntersFoV( player.get() );
		}
	}
	// restricts an entities position to within the map
	void Bound(Entity& e)
	{
		e.pos.x = (float)( max( 0, min( (int)e.pos.x, (int)(w*32 - e.dim.x ) ) ) );
		e.pos.y = (float)( max( 0, min( (int)e.pos.y, (int)(h*32 - e.dim.y ) ) ) );
	}
	void Render( SDL_Surface * screen )
	{
		auto  rCamDim   = camera->dim, // remaining camera dimension
		      cCamPos   = camera->pos, // current camera pos 
			  screenPos = Vector(0, 0);
		auto cr = camera->Rectangle();

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

			Surface::OnDraw( screen, data[tile], (int)screenPos.x, (int)screenPos.y, (int)tilePos.x, (int)tilePos.y, (int)tileDim.x, (int)tileDim.y );                                                            

			screenPos.x += tileDim.x;
			cCamPos.x   += tileDim.x;
			rCamDim.x   -= tileDim.x;

			if ( rCamDim.x <= 0)
			{
				screenPos.x = 0;
				screenPos.y += tileDim.y;
				cCamPos.y   += tileDim.y;
				cCamPos.x   = camera->pos.x;
				rCamDim.x   = camera->dim.x;
				rCamDim.y   -= tileDim.y;
			}
		}

		// render entities
		for(Entity* entity : entities)
		{
			if ( entity->model == nullptr )
				continue; 
			if ( camera->Rectangle().Intersect( entity->Rectangle() ) )
			{
				Rect c( camera->pos, camera->dim ), // c = camera rect
								e( entity->pos, entity->dim ); // e = entity rect
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
				entity->RenderFov( cr );
				Surface::OnDraw( screen, entity->model, x, y, srcX, srcY, srcW, srcH );
				entity->Render( screen, cr );
			}
		}
	}
	void KeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
	{
		player->KeyDown( sym, mod, unicode );

		switch ( sym )
		{
			case SDLK_RIGHT: camera->vel.x = 4; break;
			case SDLK_LEFT: camera->vel.x = -4; break;
			case SDLK_UP: camera->vel.y = -4; break;
			case SDLK_DOWN: camera->vel.y = 4; break;
			//case SDLK_SPACE:
			//	{
					//if ( dialog == nullptr) {
					//	dialog = new Dialog( camera.Rectangle() );
					//	dialog->CalcLines();
					//}
					//else {
					//	delete dialog;
					//	dialog = nullptr;
					//}
				//} break;
			default: break;
		}
	}	
	void KeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
	{
		player->KeyUp( sym, mod, unicode );
		switch ( sym )
		{
			case SDLK_RIGHT: camera->vel.x = 0; break;
			case SDLK_LEFT: camera->vel.x = 0; break;
			case SDLK_UP: camera->vel.y = 0; break;
			case SDLK_DOWN: camera->vel.y = 0; break;
				default: break;
		}
	}
	void CleanUp()
	{
		SDL_FreeSurface(grass); SDL_FreeSurface(dirt);
		SDL_FreeSurface(avatar);
	}
};

class MaB : public SDL_App
{
	Game g;
	SDL_Surface * screen;
	HighPerformanceTimer hpt;
	Vector screenDim;
	FpsCounter fpsCounter;
	SDL_Video video;
public:
	MaB() {}
	~MaB() {}
	
	void KeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) override
	{
		((HomeLand*)g.map)->KeyDown( sym, mod, unicode );
	}
	void KeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) override
	{
		((HomeLand*)g.map)->KeyUp( sym, mod, unicode );
	}
	bool Init() override
	{
		video.screen = screen;
		screenDim = Vector( 600, 600 );
		if((screen = SDL_SetVideoMode( (int)screenDim.x, (int)screenDim.y, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) 
		{
			return false;
		}

		g.map = new HomeLand( g );
		((HomeLand*)g.map)->Init( screen );

		hpt.InitTimer(20); 
		// _putenv("SDL_VIDEODRIVER=windib"); whats frame rate with this in?
		fpsCounter.Init( std::string( "consola.ttf" ) );
		return true;
	}
	void Render() override
	{
		((HomeLand*)g.map)->Render( screen );
		fpsCounter.Render( screen, 0, 0 );
		SDL_Flip( screen );
	}
	void Logic() override 
	{ 
		static bool once=true; if ( once ) { hpt.UpdateTime0(); once = false; }
		//static auto logicFPS = 0;
		//static auto startTime = GetTickCount();
		hpt.UpdateTime1();
		if ( hpt.DeltaTime() >= hpt.LogicTime() )
		{
			((HomeLand*)g.map)->Logic(); 
			//logicFPS++;
			hpt.UpdateFixedStep();
		}

		fpsCounter.Logic();
		//if ( GetTickCounta() - startTime >= 1000 )
		//{
		//	std::cout << logicFPS << std::endl; logicFPS=0;
		//	startTime = GetTickCount();
		//}
	}
	void Cleanup() override
	{
		((HomeLand*)g.map)->CleanUp();
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