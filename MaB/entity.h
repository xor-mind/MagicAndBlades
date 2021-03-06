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
#include "MabMisc.h"
#include "BloodEmitter.h"

using namespace boost::assign; // bring 'operator+=()' into scope

struct Entity : public EntityEventManager
{
	SDL_Video* video;
	Rect FoV;
	Vector fovDim;
	SDL_Surface * model, * healthBar;
	
	// entity properties
	int str, health;
	Vector pos, dim, vel;
	Vector remainingDistance; // used to travel to a tile
	float speed;
	bool warMode;
	bool NPC, remove;

	class Attack
	{
	public:
		Attack( Entity* e ) : melee( e ) {}
		class Melee
		{
			Entity* e;
			uint coolDownTimer;
			bool targetKilled;
		public:
			Melee( Entity* e ) : e(e), coolDownTimer(0) {}
			bool Cooldown() { return ( SDL_GetTicks() - coolDownTimer > 3000 )  ? false : true; }
			bool Attack( Entity* target ) 
			{
				if ( !Cooldown() )
					if ( e->Rectangle().Intersect( target->Rectangle() ) )
					{
						target->Damage( 3 );
						if ( target->health <= 0 )
							targetKilled = true;
						else
							targetKilled = false;
						coolDownTimer = SDL_GetTicks();
						return true;
					}
				return false; 
			}
			bool TargetKilled() const { return targetKilled; }
		};
		Melee melee;
	};
	Attack* attack;

	ParticleEmitter* particles;
public:

	Entity( ) : model(nullptr) 
	{ 
		attack = new Attack( this ); 
		particles = new BloodEmitter();
	}
	virtual ~Entity() 
	{ 
		delete particles;
		delete attack; 
	}

	Rect Rectangle() { return Rect( pos, dim ); }

	virtual bool Init(SDL_Surface* model, SDL_Surface* healthBar)
	{
		remove = false;
		warMode = false;
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

		// do we already have a path set?
		if ( remainingDistance.x != 0.f || remainingDistance.y != 0.f )
		{
			remainingDistance -= vel;

			if (remainingDistance.x == 0.f && remainingDistance.y == 0.f )
			{
				vel = Vector(0, 0);
			}
			else
			{
				float speed_x = sgn<float>(remainingDistance.x) * min( abs(remainingDistance.x), speed ),
					  speed_y = sgn<float>(remainingDistance.y) * min( abs(remainingDistance.y), speed );
				vel.x = speed_x;
				vel.y = speed_y;
				pos += vel;
			}
		}
		else
			pos += vel;
		
		
		FoV.left = (int)( pos.x + dim.x/2 - fovDim.x );
		FoV.right = (int)( pos.x + dim.x/2 + fovDim.x );
		FoV.top = (int)( pos.y + dim.y/2 - fovDim.y );
		FoV.bottom = (int)( pos.y + dim.y/2 + fovDim.x );

		particles->Update();
	}
	virtual void Render( SDL_Surface* dest, Entity* camera )
	{
		if ( model == nullptr )
				return; 
		Rect cr = camera->Rectangle();
		if ( cr.Intersect( Rectangle() ) )
		{
			int render_x = (int)( pos.x - cr.left ),
				render_y = (int)( pos.y - cr.top );
			//RenderFov( cr );
			Surface::OnDraw( dest, model, render_x, render_y );
			int healthBarWidth = (int)( (health/(float)str) * healthBar->w );
			Surface::OnDraw( dest, healthBar, render_x, render_y - healthBar->h - 2, 0, 0, healthBarWidth, healthBar->h );
			particles->Render( dest, cr );
		}
	}
	
	void Damage( int d ) 
	{ 
		int x = (int)(pos.x + rand()%(int)dim.x),
			y = (int)(pos.y + rand()%(int)dim.y);
		particles->Init( x, y );
		health -= d; 
		if (health <= 0 ) remove = true; 
		std::cout << "damaged called!" << std::endl;
	}

	void MoveToPlayer( Entity* target )
	{
		vel = Vector( 0, 0 );

		int dx = (int)( pos.x - target->pos.x ),
			dy = (int)( pos.y - target->pos.y );
		int dist_x = abs( dx ), dist_y = abs( dy );


		vel.x = -sgn( dx ) * speed;
		vel.y = -sgn( dy ) * speed;
	}
	
	// move x tiles by y tiles
	void MoveTiles( int x, int y )
	{
		if ( !isMoving() )
		{
			remainingDistance = Vector(x*32.f, y*32.f);
			vel = Vector(0.f, 0.f);
		}
	}

	bool isMoving()
	{
		if ( remainingDistance.x == 0.f && remainingDistance.y == 0.f )
			return false;
		else 
			return true;
	}

	void Strength( int str ) { this->str = str; health = str; }

	void ToggleWarMode()
	{
		warMode = !warMode;
	}

	bool isNPC() { return NPC; }

	bool Remove() { return remove; }
};

typedef std::vector<Entity*> EntityVector;
typedef std::list<Entity*> EntityList;
#endif



