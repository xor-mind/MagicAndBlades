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
#include "MabMisc.h"

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
	Vector remainingDistance; // used to travel to a tile
	float speed;
	bool doneMoving;
	class Attack
	{
	public:
		Attack( Entity* e ) : melee( e ) {}
		class Melee
		{
			Entity* e;
			uint coolDownTimer;
		public:
			Melee( Entity* e ) : e(e), coolDownTimer(0) {}
			bool Cooldown() { return ( SDL_GetTicks() - coolDownTimer > 3000 )  ? false : true; }
			bool Attack( Entity* target ) 
			{
				if ( !Cooldown() )
					if ( e->Rectangle().Intersect( target->Rectangle() ) )
					{
						target->Damage( 3 );
						coolDownTimer = SDL_GetTicks();
					}
				return false; 
			}
		};
		Melee melee;
	};
	Attack* attack;
public:

	Entity( Game& game ) : model(nullptr), g( game ) { attack = new Attack( this ); }
	virtual ~Entity() { delete attack; }

	Rect Rectangle() { return Rect( pos, dim ); }

	virtual bool Init(SDL_Surface* model, SDL_Surface* healthBar)
	{
		doneMoving = false;
		remainingDistance = Vector(0, 0); 
		this->model = model;
		this->healthBar = healthBar;
		return true;
	}
	void RenderFov( Rect & cam)
	{
		Rect r = FoV.SubtractPosition( cam );
		video->renderPerimiter(&r);
	}
	virtual void Logic()
	{
		ProcessEvent();

		// after moving an automated distance we need to set the vel to zero, but this must be after collision detection
		// hence this weirdness.
		if ( doneMoving == true ) {
			vel = Vector( 0, 0 );
			doneMoving = false;
		}
		// do we already have a path set?
		if ( remainingDistance.x != 0.f || remainingDistance.y != 0.f )
		{
			float speed_x = sgn<float>(remainingDistance.x) * min( abs(remainingDistance.x), speed ),
				  speed_y = sgn<float>(remainingDistance.y) * min( abs(remainingDistance.y), speed );
			vel.x = speed_x;
			vel.y = speed_y;
			remainingDistance.x -= speed_x; 
			remainingDistance.y -= speed_y;
			pos += vel;
			doneMoving = true;
		}
		else
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
	
	void Damage( int d ) { health -= d; }

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
	
	// move x tiles by y tiles
	void MoveTiles( int x, int y )
	{
		if ( remainingDistance.x == 0.f && remainingDistance.y == 0.f )
		{
			remainingDistance = Vector(x*32.f, y*32.f);
			doneMoving = false;
		}
	}

	void Strength( int str ) { this->str = str; health = str; }
};

typedef std::vector<Entity*> EntityVector;

#endif



