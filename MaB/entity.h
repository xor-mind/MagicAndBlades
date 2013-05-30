#ifndef ENTITY_H
#define ENTITY_H

#include "types.h"
#include <algorithm>
#include "SDL.h"
#include "SDL_surface.h"
#include <list>
#include <boost/assign/std/vector.hpp> // for 'operator+=()'
#include "SDL_Gfx.h"
#include "Rectangle.h"
#include "Dialog.h"
#include "EntityEvents.h"
#include "Game.h"

using namespace boost::assign; // bring 'operator+=()' into scope

struct Entity : public EntityEventManager
{
	Game& g;
	SDL_Video* video;
	Rect FoV;
	Vector fovDim;
	SDL_Surface * model, * healthBar;
	
	// entity properties
	int str, health;
	Vector pos, dim, vel;
	float speed;

public:

	Entity( Game& game ) : model(nullptr), g( game ) {}
	virtual ~Entity() {}

	Rect Rectangle() { return Rect( pos, dim ); }

	virtual bool Init(SDL_Surface* model, SDL_Surface* healthBar)
	{
		this->model = model;
		this->healthBar = healthBar;
		return true;
	}
	void RenderFov( Rect & cam)
	{
		Rect r = FoV.SubtractPosition( cam );
		video->renderPerimiter(&r);
	}
	void Logic()
	{
		ProcessEvent();
		pos += vel;
		FoV.left = (int)( pos.x + dim.x/2 - fovDim.x );
		FoV.right = (int)( pos.x + dim.x/2 + fovDim.x );
		FoV.top = (int)( pos.y + dim.y/2 - fovDim.y );
		FoV.bottom = (int)( pos.y + dim.y/2 + fovDim.x );
	}
	virtual void Render( SDL_Surface* dest )
	{
		if ( model == nullptr )
				return; 
		Rect cr = g.camera->Rectangle();
		if ( cr.Intersect( Rectangle() ) )
		{
			int render_x = (int)( pos.x - cr.left ),
				render_y = (int)( pos.y - cr.top );
			//RenderFov( cr );
			Surface::OnDraw( dest, model, render_x, render_y );
			int healthBarWidth = (int)( (health/(float)str) * healthBar->w );
			Surface::OnDraw( dest, healthBar, render_x, render_y - healthBar->h - 2, 0, 0, healthBarWidth, healthBar->h );
		}
	}
	
	void MoveToPlayer( Entity* target )
	{
		vel = Vector( 0, 0 );

		int dx = (int)( pos.x - target->pos.x ),
			dy = (int)( pos.y - target->pos.y );
		int dist_x = abs( dx ), dist_y = abs( dy );

		if ( dist_x > dist_y )
		{
			vel.x = -sgn( dx ) * speed;
		}
		else
		{
			vel.y = -sgn( dy ) * speed;
		}
	}

	void Strength( int str ) { this->str = str; health = str; }
};

typedef std::vector<Entity*> EntityVector;

#endif



