#ifndef MAB_H
#define MAB_H

#include "SDL_app.h"
#include "types.h"
#include "winapi_timer.h"
#include <string>
#include <vector>
#include <iostream>
#include "SDL_FpsCounter.h"
#include "SDL_Text.h"
#include "SDL_Gfx.h"
#include "Dialog.h"
#include "Map.h"


class MaB : public SDL_App
{
	SDL_Surface * screen;
	HighPerformanceTimer hpt, tearTimer;
	Vector screenDim;
	FpsCounter fpsCounter;
	SDL_Video video;
	Map* map;
		  
public:
	MaB() {}
	~MaB() {}
	
	bool Init() override
	{
		video.screen = screen;
		// 1280 = 20*64, 768 = 12*64
		screenDim = Vector( 20*64, 12*64 );

		if(( screen = SDL_SetVideoMode( (int)screenDim.x, (int)screenDim.y, 32,   SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) //SDL_FULLSCREEN |
		{
			return false;
		}

		map = new HomeLand();
		((HomeLand*)map)->Init( screen );
		tearTimer.InitTimer(60);
		hpt.InitTimer(20); 
		// _putenv("SDL_VIDEODRIVER=windib"); whats frame rate with this in?
		fpsCounter.Init( std::string( "consola.ttf" ) );
		tearTimer.UpdateTime0();
		return true;
	}

	void Render() override
	{
		((HomeLand*)map)->Render( screen );
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
			((HomeLand*)map)->Logic(); 
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
		((HomeLand*)map)->CleanUp();
		delete map;
	}

	void KeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) override
	{
		if ( sym == SDLK_ESCAPE) {
			SDL_FreeSurface( screen );
			screen = SDL_SetVideoMode( screenDim.x, screenDim.y, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
		}
		((HomeLand*)map)->KeyDown( sym, mod, unicode );
	}
	void KeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) override
	{
		((HomeLand*)map)->KeyUp( sym, mod, unicode );
	}
	void LButtonDown(int mX, int mY) override 
	{
		((HomeLand*)map)->LButtonDown( mX, mY );
	} 
 
	void LButtonUp(int mX, int mY) override {}
 
	void RButtonDown(int mX, int mY) override 
	{
		((HomeLand*)map)->RButtonDown( mX, mY );
	}
 
	void RButtonUp(int mX, int mY) override {}

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