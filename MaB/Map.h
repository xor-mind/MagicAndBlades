#ifndef MAP_H
#define MAP_H

#include "Player.h"
#include "Agggrool.h"

#include <algorithm>
#include <memory>

class Map
{
protected:
	static const int tilew = 32, tileh = 32;
	uint w,h;  // map width and height
public:
	struct Tile
	{
		Tile( SDL_Surface* data, int propId ) : data(data), propId(propId) {}
		SDL_Surface* data;
		int propId;

		SDL_Surface* Render( SDL_Surface* dest, int x, int y ) 
		{
			Surface::OnDraw( dest, data, x, y, propId*tilew, 0, tilew, tileh );   
			return nullptr; 
		}
	};
public:
	virtual ~Map() {}
	class Cell
	{ 
	public:
	};

	std::vector<Tile> data;
};

class HomeLand : public Map
{
private:
	SDL_Surface * grass, * dirt, * avatar, *healthBar, *terrainProps;
	std::unique_ptr<Player> player;
	std::unique_ptr<AggGrool> aggGrool;
	EntityVector entities;
	SDL_Rect clipRect;
	SDL_Video video;
	Game& g;
	Entity* camera;
public:
	HomeLand( Game& game) : g( game )
	{ 
		w = h = 80; 
		//camera.reset( new Entity( g ) );
		camera = g.camera = new Entity( g );
		player.reset( new Player( g ) );
		aggGrool.reset( new AggGrool( g ) );
	}
	~HomeLand() { delete g.camera; }

	void Init( SDL_Surface * display )
	{
		
		video.screen = display;
		camera->pos = camera->vel = Vector( 0, 0 );
		camera->dim = Vector( 32*16, 32*8 ); // map is 512x256

		clipRect.x = 0; clipRect.y =  0, clipRect.w = (Uint16)camera->dim.x; clipRect.h = (Uint16)camera->dim.y;
		video.clipRect = clipRect;
		SDL_SetClipRect(display, &clipRect);

		grass        = Surface::BmpLoad( "./art/grass01.bmp" );
		dirt         = Surface::BmpLoad( "./art/grass02.bmp" );
		terrainProps = Surface::BmpLoad( "./art/terrainProps.bmp" );

		for ( int y = 0; y < (int)h; ++y )
			for ( int x = 0; x < (int)w; ++x )
			{
				if ( rand()%10 < 2 )
				{
					data.push_back( Tile( terrainProps, rand()%4 ) );
				}
				else 
				{
					if ( y % 2 != 0 )	
						data.push_back( x % 2 != 0 ? Tile(dirt, 0) : Tile(grass, 0) ); 
					else 
						data.push_back( x % 2 != 0 ? Tile(grass, 0) : Tile(dirt, 0));
				}
			}

		avatar = Surface::BmpLoad("./art/avatar01.bmp");
		healthBar = Surface::BmpLoad("./art/healthBar.bmp");
		player->Init(avatar, healthBar);
		aggGrool->Init(avatar, healthBar);
		aggGrool->pos = Vector(32*8,32*8);
		entities.push_back( camera );
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
		int pos_y = (int) camera->pos.y, 
			max_y = (int)( camera->pos.y + camera->dim.y ),
			max_x = (int)( camera->pos.x + camera->dim.x ),
		   offs_x = (int) camera->pos.x % tilew,
		   offs_y = (int) pos_y % tileh;

		for ( ; pos_y < ( max_y + offs_y ); pos_y += tileh )
		{
			for ( int pos_x = (int) camera->pos.x ; pos_x < ( max_x + offs_x ) ; pos_x += tilew )
			{
				int tile = pos_y / tileh * Map::w + pos_x / tilew;
				int offs_y = pos_y % tileh,
					render_y = (int)( pos_y - camera->pos.y - offs_y ),
					render_x = (int)( pos_x - camera->pos.x - offs_x );
				data[tile].Render(screen, render_x, render_y);                                                            
			}
		}

		// render entities
		for(Entity* entity : entities)
		{
			entity->Render( screen );	
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

	void LButtonDown(int mX, int mY)  
	{
		player->LButtonDown( mX, mY );
	} 
	void RButtonDown(int mX, int mY)  
	{
		player->RButtonDown( mX, mY );
	}

	void CleanUp()
	{
		SDL_FreeSurface(grass); SDL_FreeSurface(dirt);
		SDL_FreeSurface(avatar); SDL_FreeSurface( healthBar );
	}
};


#endif