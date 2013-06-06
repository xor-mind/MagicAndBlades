#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Combat.h"

class Player : public Entity
{
	Combat combat;
public:

	EntityEventList dialogEvents;

	Player( Game& g) : Entity(g), combat(this)
	{
		Strength(18);
		pos = vel = Vector(0,0);
		speed = 4.f;
		dim = Vector(32,32);
		fovDim = Vector(100, 100);
		NPC = false;
	}
	
	void KeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
	{
		if ( dialogEvents.size() )
		{
			for( EntityEvent* e : dialogEvents ) e->KeyDown( sym, mod, unicode );
		}
		else
			switch ( sym )
			{
				case SDLK_d: vel.x = speed; break;
				case SDLK_a: vel.x = -speed; break;
				case SDLK_w: vel.y = -speed; break;
				case SDLK_s: vel.y = speed; break;
				case SDLK_i: MoveTiles(0,-1); break;
				case SDLK_j: MoveTiles(-1,0); break;
				case SDLK_k: MoveTiles(0,1); break;
				case SDLK_l: MoveTiles(1, 0); break;
				case SDLK_TAB: ToggleWarMode(); break;
					default: break;
			}
	}
	void KeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
	{
		if ( dialogEvents.size() )
		{
			for( EntityEvent* e : dialogEvents ) e->KeyUp( sym, mod, unicode );
		}
		else
			switch ( sym )
			{
				case SDLK_d: vel.x = 0; break;
				case SDLK_a: vel.x = 0; break;
				case SDLK_w: vel.y = 0; break;
				case SDLK_s: vel.y = 0; break;
				default: break;
			}
	}
	void LButtonDown(int mX, int mY, Entity* e)  
	{
		if ( dialogEvents.size() )
			for( EntityEvent* e : dialogEvents ) 
				e->LButtonDown( mX, mY );
		else
		{
			if ( warMode )
			{
				if ( e )
					combat.target = e;
			}
		}
	} 
	void RButtonDown(int mX, int mY)  
	{
		for( EntityEvent* e : dialogEvents ) 
			e->RButtonDown( mX, mY );
	}
	void Logic() override
	{
		combat.Logic();
		Entity::Logic();

		for( auto itr = dialogEvents.begin(), end=dialogEvents.end();
			 itr != end;  ) 
		{
			if ( (*itr)->Complete() ) {
				delete (*itr);
				itr = dialogEvents.erase(itr);
				continue;
			}
			++itr;
		}

		for( EntityEvent* e : dialogEvents ) 
			e->Logic();
	}
	void Render( SDL_Surface* dest ) override
	{
		Rect cr = g.camera->Rectangle();
		if ( cr.Intersect( Rectangle() ) )
		{
			for( EntityEvent* e : dialogEvents ) 
				e->Render( dest, cr );
		}
		Entity::Render( dest );
	}
	void MultipleChoiceChunkEvent(MultipleChoiceChunk* mcc) override
	{
		dialogEvents.push_back( (EntityEvent*)mcc );
		vel = Vector(0, 0);
	}
};

#endif