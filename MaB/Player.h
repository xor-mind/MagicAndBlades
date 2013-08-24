#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Combat.h"

class Player : public Entity
{
	Combat combat;
	Sprite exclamation;
	Sprite moveLeft, moveRight, moveTop, moveBot;
public:

	EntityEventList dialogEvents;

	Player() :  combat(this)
	{
		Strength(18);
		pos = vel = Vector(0,0);
		speed = 4.f;
		dim = Vector(32,32);
		fovDim = Vector(100, 100);
		NPC = false;

		SDL_Surface* s = Surface::BmpLoad( "./art/red_exclamation.bmp");
		Surface::PinkTransparent(s);
		exclamation.surfaces.push_back( s );
		exclamation.blinkingDelay = 444;
	}
	
	bool Init(SDL_Surface* model, SDL_Surface* healthBar) override
	{
		Entity::Init( model, healthBar );
		dim.x = 16;
		int x = 0; 
		for ( ; x < 16*3; x+=16 )
		{
			SDL_Surface* s = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 32, 32, 0, 0, 0, 0);
			Surface::PinkTransparent( s );
			Surface::OnDraw( s, model, 0, 0, x, 0, 16, 32 );
			moveLeft.surfaces.push_back( s );
			moveRight.surfaces.push_back( SDL_Video::flip_surface( s, 0 ) );
		}
		for ( ; x < 16*6; x+=16 )
		{
			SDL_Surface* s = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 32, 32, 0, 0, 0, 0);
			Surface::PinkTransparent( s );
			Surface::OnDraw( s, model, 0, 0, x, 0, 16, 32 );
			moveBot.surfaces.push_back( s );
		}
		for ( ; x < 16*9; x+=16 )
		{
			SDL_Surface* s = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 32, 32, 0, 0, 0, 0);
			Surface::PinkTransparent( s );
			Surface::OnDraw( s, model, 0, 0, x, 0, 16, 32 );
			moveTop.surfaces.push_back( s );
		}
		moveLeft.delayTime = 100;
		moveBot.delayTime = 100;
		moveTop.delayTime = 100;
		moveRight.delayTime = 100;
	
		return true;
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

		if ( vel.x > 0.f ) {
			moveRight.Update();
		}
		else if ( vel.x < 0.f ) {
			moveLeft.Update();
		}
		if ( vel.y > 0 ) {
			moveBot.Update();
		}
		else if ( vel.y < 0 ) {
			moveTop.Update();
		}	

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

		exclamation.Update();
	}
	void Render( SDL_Surface* dest, Entity* camera ) override
	{
		Rect cr = camera->Rectangle();
		if ( cr.Intersect( Rectangle() ) )
		{
			for( EntityEvent* e : dialogEvents ) 
				e->Render( dest, cr );
		}

		if ( model == nullptr )
				return; 

		if ( cr.Intersect( Rectangle() ) )
		{
			int render_x = (int)( pos.x - cr.left ),
				render_y = (int)( pos.y - cr.top );
			//RenderFov( cr );
			if ( vel.x > 0.f ) {
				moveRight.Render(dest, render_x, render_y);
			}
			else if ( vel.x < 0.f ) {
				moveLeft.Render(dest, render_x, render_y);
			}
			else if ( vel.y > 0 ) {
				moveBot.Render(dest, render_x, render_y);
			}
			else if ( vel.y < 0 ) {
				moveTop.Render(dest, render_x, render_y);
			}	
			else
			{
				Surface::OnDraw( dest, moveBot.surfaces[1], render_x, render_y );
			}
			int healthBarWidth = (int)( (health/(float)str) * healthBar->w );
			Surface::OnDraw( dest, healthBar, (int)(render_x + dim.x/2 -  healthBar->w/2), render_y - healthBar->h - 2, 0, 0, healthBarWidth, healthBar->h );
			
			if ( warMode )
			{
				int x = (int)(pos.x - cr.left +dim.x/2 - 8), 
					y = (int)(pos.y - cr.top - 28);
				exclamation.Render( dest, x, y );
			}
			if ( combat.target )
			{
				int x = (int)(combat.target->pos.x - cr.left + combat.target->dim.x/2 - 8), 
					y = (int)(combat.target->pos.y - cr.top - 28);
				exclamation.Render( dest, x, y );
			}			
			particles->Render( dest, cr );
		}


	}
	void MultipleChoiceChunkEvent(MultipleChoiceChunk* mcc) override
	{
		dialogEvents.push_back( (EntityEvent*)mcc );
		vel = Vector(0, 0);
	}
};

#endif