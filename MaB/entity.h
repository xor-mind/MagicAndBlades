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
	SDL_Video* video;
	Rect FoV;
	Vector fovDim;
	SDL_Surface * model;
	Game& g;
	Entity( Game& game ) : model(nullptr), g( game ) {}
	virtual ~Entity() {}
	Vector pos, dim, vel;
	Rect Rectangle() { return Rect( pos, dim ); }

	void RenderFov( Rect & cam)
	{
		FoV.left = (int)( pos.x + dim.x/2 - fovDim.x );
		FoV.right = (int)( pos.x + dim.x/2 + fovDim.x );
		FoV.top = (int)( pos.y + dim.y/2 - fovDim.y );
		FoV.bottom = (int)( pos.y + dim.y/2 + fovDim.x );
		Rect r = FoV.SubtractPosition( cam );
		video->renderPerimiter(&r);
	}
	void Logic()
	{
		ProcessEvent();
		pos += vel;
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
			RenderFov( cr );
			Surface::OnDraw( dest, model, render_x, render_y );
		}
	}
	//void PushEvent(EntityEvent* Event) 
	//{
	//	entityEvents.push_back( Event );
	//}
	virtual bool Init(SDL_Surface* s)
	{
		model = s;
		return true;
	}
};

typedef std::vector<Entity*> EntityVector;

class Monster : public Entity
{
public:
	Monster(Game& g) : Entity(g) {}
	virtual ~Monster() {}

	void Logic()
	{
		Entity::Logic();
	}

	// some properties of monsters


	void Immortal( bool b ) { immortal = b; }
	void InstaKill( bool b ) { instaKill = b; }

	bool immortal;
	bool instaKill;
};

#endif



