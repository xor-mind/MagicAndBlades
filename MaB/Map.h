#ifndef MAP_H
#define MAP_H

#include "Player.h"
#include "Agggrool.h"
#include "Sheep.h"

#include <algorithm>
#include <memory>

struct EntityFactory
{
	virtual ~EntityFactory() {}
	virtual Entity* create() = 0;
};

struct SheepFactory : public EntityFactory
{
	SDL_Surface* model, * healthBar;
	Entity* create() override
	{
		Entity * e = new Sheep();
		e->Init( model, healthBar);
		return e;
	}

	static SheepFactory* instance() { return & sheepFactory(); }
private:
	static SheepFactory& sheepFactory() { static SheepFactory f; return f; }
	SheepFactory() {}
};

class Map
{
protected:
	static const int tilew = 32, tileh = 32;
	uint w,h;  // map width and height
	EntityList entities;

public:
	struct Tile
	{
		enum typeType { eNormal, eNoGo };
		typeType type;
		Tile( SDL_Surface* data, int propId ) 
			: data(data), propId(propId) { type = eNormal; }
		Tile( SDL_Surface* data, int propId, typeType type ) 
			: data(data), propId(propId), type( type ) {}
		SDL_Surface* data;
		int propId;
		//int pos_x, pos_y;
		Rect r;

		bool NoGo() { return ( type == typeType::eNoGo ? true : false ); }
		SDL_Surface* Render( SDL_Surface* dest, int x, int y ) 
		{
			Surface::OnDraw( dest, data, x, y, propId*tilew, 0, tilew, tileh );   
			return nullptr; 
		}
	};
	std::vector<Tile> data;

	struct Spawn
	{
		uint spawnDelay;
		uint maxCount; 
		Entity* type;
		uint tile_x, tile_y;

		EntityFactory * factory;
		EntityList spawned;
		Map* map;

		Spawn( Map* map, uint spawnDelay, uint maxCount, uint tile_x, uint tile_y, EntityFactory * factory )
			: map(map), spawnDelay( spawnDelay ), maxCount( maxCount ), tile_x( tile_x ), tile_y( tile_y ), factory( factory )
		{
		}
		
		void SpawnAll()
		{
			for (uint i = spawned.size(); i < maxCount; ++i )
			{
				Entity * e = factory->create();
				e->pos = Vector(32*8,32*10);
				map->AddEntity( e );
			}
		}
	};

public:
	Map() {}
	virtual ~Map() {}

	// returns true if an entity is standing on any no go tiles
	bool EntityCollision( Entity* e )
	{
		static int tiles[4];
		int tileCount = 0;
		int pos_x = (int)e->pos.x, pos_y = (int)e->pos.y;
		int offs_x =  pos_x % tilew, offs_y =  pos_y % tileh;

		// collect all the tile's we're intersecting with
		tiles[ tileCount++ ] = pos_y / tileh * Map::w + pos_x / tilew;

		if ( (offs_x + e->dim.x) > tilew )
			tiles[tileCount++] = tiles[0] + 1;
		if ( offs_y > 0 )
			tiles[tileCount++] = tiles[0] + Map::w;
		if( ( (offs_x + e->dim.x) > tilew ) && (offs_y > 0) )
			tiles[tileCount++] = tiles[0] + Map::w + 1;

		// see if we're intersecting with any no go tiles and work out the right collision response
		for ( int i = 0; i < tileCount; ++i )
		{
			if ( data[tiles[i]].NoGo() )
				return true;
		}

		return false;
	}

	void Collision( Entity* e )
	{
		// how collision detection works.
		// After an entity has moved,
		// check if there's any collisions.
		// if so, revert one component ( x component ) of the position
		// again check for any collisions.
		// if no, then place the entity at the edge of the offending tile,
		// which is gleaned from the position component changed.
		//
		// if there is a collision, negate the reversion and test the other
		// position component (y). if there's still a collision after reverting 
		// both components by themselves, then we know there's a collision in both
		// dimensions and figure out where to place the object in the x/y plane.

		bool xPosChanged = ( e->vel.x == 0 ? true : false ),
			 yPosChanged = ( e->vel.y == 0 ? true : false );
		
		if ( xPosChanged && yPosChanged ) // entity has not moved, no collision
			return;	

		bool collision = EntityCollision( e );

		if ( collision )
		{
			int offs_x =  (int)e->pos.x % tilew, offs_y =  (int)e->pos.y % tileh;
			int tempPos_x, tempPos_y;

			// first change x position
			if ( !xPosChanged  )
			{
				tempPos_x = (int)e->pos.x;
				e->pos.x = e->pos.x - e->vel.x;
				collision = EntityCollision( e );
				if ( collision ) {
					e->pos.x = (float)tempPos_x;
				}
				else
				{
					int dim_offs_x = int( tempPos_x + e->dim.x ) % tilew;
					e->pos.x = tempPos_x - ( e->vel.x > 0 ? dim_offs_x : -(tilew - offs_x) );  //= //(float)( tempPos_x - ( e->vel.x > 0 ? ( offs_x - e->dim.x ) : -(tilew - offs_x) ) );
					return;
				}
			}

			// second, change y position
			tempPos_x = (int)e->pos.x;
			tempPos_y = (int)e->pos.y;
			e->pos.y = e->pos.y - e->vel.y; 
			collision = EntityCollision( e );
			if ( collision )
			{
				int dim_offs_x = int( tempPos_x + e->dim.x ) % tilew;
				e->pos.x = tempPos_x - ( e->vel.x > 0 ? dim_offs_x : -(tilew - offs_x) );  
				e->pos.y = (float)( tempPos_y - ( e->vel.y > 0 ? offs_y : -(tilew - offs_y) ) );
				return;
			}
			else
			{
				e->pos.y = (float)( tempPos_y - ( e->vel.y > 0 ? offs_y : -(tilew - offs_y) ) );
				return;
			}
		}
	}

	int Test() { return 5; }

	void AddEntity( Entity* e )
	{
		entities.push_back(e);
	}
};

class HomeLand : public Map
{
private:
	SDL_Surface * grass, * dirt, * avatar,  *sheepSurface, *healthBar, *terrainProps, *muhammod;
	std::unique_ptr<Player> player;
	std::unique_ptr<AggGrool> aggGrool;
	std::unique_ptr<Sheep> sheep;
	
	SDL_Rect clipRect;
	SDL_Video video;
	
	Entity* camera;
	Spawn* spawn;
public:
	HomeLand( ) 
	{ 
		w = h = 80; 
		//camera.reset( new Entity( g ) );
		camera = new Entity( );
		player.reset( new Player( ) );
		aggGrool.reset( new AggGrool(  ) );
		sheep.reset( new Sheep( ) );
	}
	~HomeLand() { delete camera; }


	void Init( SDL_Surface * display )
	{
		
		video.screen = display;
		camera->pos = camera->vel = Vector( 0, 0 );
		camera->dim = Vector( (float)display->w, (float)display->h ); 

		clipRect.x = 0; clipRect.y =  0, clipRect.w = (Uint16)camera->dim.x; clipRect.h = (Uint16)camera->dim.y;
		video.clipRect = clipRect;
		SDL_SetClipRect(display, &clipRect);

		muhammod     = Surface::BmpLoad("./art/muhammod.bmp");
		grass        = Surface::BmpLoad( "./art/grass03.bmp" );
		dirt         = Surface::BmpLoad( "./art/grass03.bmp" );
		terrainProps = Surface::BmpLoad( "./art/terrainProps.bmp" );
		

		for ( int y = 0; y < (int)h; ++y )
			for ( int x = 0; x < (int)w; ++x )
			{
				if ( ( rand()%10 < 2 ) && ( (x!=0) || (y!=0) ) && ( (x!=8) || (y!=8) ) && ( (x!=8) || (y!=10) ) )
				{
					data.push_back( Tile( terrainProps, rand()%4, Map::Tile::eNoGo ) );
				}
				else 
				{
					if ( y % 2 != 0 )	
						data.push_back( x % 2 != 0 ? Tile(dirt, 0) : Tile(grass, 0) ); 
					else 
						data.push_back( x % 2 != 0 ? Tile(grass, 0) : Tile(dirt, 0));
				}
			}

		avatar       = Surface::BmpLoad("./art/avatar01.bmp");
		healthBar    = Surface::BmpLoad("./art/healthBar.bmp");
		sheepSurface = Surface::BmpLoad("./art/sheep.bmp");
		Surface::PinkTransparent( sheepSurface );
		Surface::PinkTransparent( avatar );

		SheepFactory::instance()->healthBar = healthBar;
		SheepFactory::instance()->model     = sheepSurface;

		player->Init(muhammod, healthBar);
		
		aggGrool->Init(avatar, healthBar);
		aggGrool->pos = Vector(32*8,32*8);
		
		//sheep->Init( sheepSurface, healthBar );
		//sheep->pos = Vector(32*8,32*10);
		spawn = new Spawn( this, 20000, 5, 8, 10, SheepFactory::instance() );
		spawn->SpawnAll();
		entities.push_back(( (Entity*)player.get() ));
		entities.push_back(( (Entity*)aggGrool.get() ));
		//entities.push_back(( (Entity*)sheep.get() ));

		for( Entity* e: entities )
		{
			e->video = &video;
		}
	}

	void Logic()
	{

		//camera->pos += camera->vel;
		
		for( auto e = entities.begin(), end = entities.end(); e != end;  )
		{
			if ( (*e)->Remove() )
			{
				e = entities.erase( e );
				continue;
			}
			(*e)->Logic();
			Bound(*(*e));
			Collision( (*e) );
			++e;
		}

		int offs_x =  (int)player->pos.x % tilew, offs_y =  (int)player->pos.y % tileh;
		// adjust player position to get through tiles
		if ( player->vel.x != 0.f )
		{
			if ( offs_y != 0 )
				if ( offs_y <= 4 )
				{
					player->pos.y -= 1;
				}
				else if ( offs_y > 27 ) 
				{
					player->pos.y += 1;
				}
		}
		if ( player->vel.y != 0.f )
		{
			if ( offs_x != 0 )
				if ( offs_x < 4 )
				{
					player->pos.x -= 1;
				}
				else if ( offs_x > 27 ) 
				{
					player->pos.x += 1;
				}
		}

		CenterCamera( player.get() );
		Bound( *camera );

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
		SDL_FillRect(screen, /* In the window... */
		     NULL,   /* The entire window (dest rect NULL for short) */
		     0); 
		/*int pos_y = (int) camera->pos.y, 
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
		}*/

		// render entities
		for(Entity* entity : entities)
		{
			entity->Render( screen, camera );	
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
		Entity* ent = nullptr;
		float map_mx = (float)mX + camera->pos.x,
			  map_my = (float)mY + camera->pos.y;
		for ( Entity* e : entities )
		{
			if ( map_mx >= e->pos.x && map_mx < e->pos.x + e->dim.x )
				if ( map_my >= e->pos.y && map_my <  e->pos.y + e->dim.y )
				{
					ent = e;
					break;
				}
		}
		player->LButtonDown( mX, mY, ent );
	} 
	void RButtonDown(int mX, int mY)  
	{
		player->RButtonDown( mX, mY );
	}

	void CleanUp()
	{
		SDL_FreeSurface(grass); SDL_FreeSurface(dirt);
		SDL_FreeSurface(avatar); SDL_FreeSurface( healthBar );
		SDL_FreeSurface(sheepSurface);
	}

	void CenterCamera( Entity* e )
	{
		//camera->pos.x = e->pos.x + e->dim.x/2 - camera->dim.x/2; 
		//camera->pos.y = e->pos.y + e->dim.y/2 - camera->dim.y/2; 

		// construct a 75x75 box that the player can move around in
		int boxSize = 50;
		int x1 = (int)( camera->pos.x + camera->dim.x/2 - boxSize ),
			y1 = (int)( camera->pos.y + camera->dim.y/2 - boxSize );
		int x2 = x1 + (boxSize<<1),
			y2 = y1 + (boxSize<<1);
		if ( e->pos.x < x1 )
			camera->pos.x -= e->speed;
		if ( e->pos.y < y1 )
			camera->pos.y -= e->speed;
		if ( e->pos.x + e->dim.x > x2 )
			camera->pos.x += e->speed;
		if ( e->pos.y + e->dim.y > y2 )
			camera->pos.y += e->speed;
	}
};


#endif