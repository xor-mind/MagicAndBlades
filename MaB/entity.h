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
		pos += vel;

		ProcessEvent();
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
	// some properties of monsters
	void Immortal( bool b ) { immortal = b; }
	void InstaKill( bool b ) { instaKill = b; }

	bool immortal;
	bool instaKill;
};

class Player : public Entity
{
public:

	int health;
	EntityEvents dialogEvents;

	Player( Game& g) : Entity(g), health(100) 
	{
		pos = vel = Vector(0,0);
		dim = Vector(32,32);
		fovDim = Vector(100, 100);
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
				case SDLK_d: vel.x = 4; break;
				case SDLK_a: vel.x = -4; break;
				case SDLK_w: vel.y = -4; break;
				case SDLK_s: vel.y = 4; break;
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
	void LButtonDown(int mX, int mY)  
	{
		for( EntityEvent* e : dialogEvents ) 
			e->LButtonDown( mX, mY );
	} 
	void RButtonDown(int mX, int mY)  
	{
		for( EntityEvent* e : dialogEvents ) 
			e->RButtonDown( mX, mY );
	}
	void Logic()
	{
		Entity::Logic();
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

class AggGrool : public Monster
{
public:
	int health;
	MultipleChoice mc; 
	AggGrool( Game& g) : Monster(g), health(100) 
	{
		pos = vel = Vector(0,0);
		dim = Vector(32,32);
		fovDim = Vector(100, 100);
	}

	bool Init(SDL_Surface* s) override
	{
		Immortal(true);
		InstaKill(true);
		//Aggro(new AttackSecond());

		Str question = "If your first born son became ill and you were an unmarried old man whose girfriend refused to have him around, who would you choose?";
		Strs answers;
		answers += "I have to live my own life, my son must deal with the world on his own terms.",
				   "No matter what his well being comes first.";
		mc.Init(this, question, answers);

		return Entity::Init(s);
	}
	void Logic()
	{
		Entity::Logic();
	}
	void PlayerEntersFoV(Player* p)
	{
		//if ( HasCompletedQuest(p, ThreeQuestions )
		{
			//if ( AutoQuest(p) )
			{
				mc.AskPlayer(p);
			}
		}
	}
	void SingleQuestion(Player* p)
	{
		static Str intro = "You've come to a forbidden zone, answer me these three questions and I may let you live.";

		//TellPlayer(intro);
		

		//if ( playerChoice.IsAnswered() )
		//{
		//	if ( playerChoice.Answer() == 1 )
		//	{
		//		Attack(p);
		//	}
		//	else
		//	{
		//		GivePlayer(p,crown);
		//		SingleQuestion.Complete(true);
		//	}
		//}

		// some how I need to implement waiting.. like the daemon is
		// waiting for the players response. different threads
		// is the onlything that comes to mind?
		
	}

	void MultipleChoiceChunkEvent(MultipleChoiceChunk* mcc) override
	{
		if ( mcc->SelectedAnswer() >= 0 )
		{
			// do something
			__asm nop; 
			delete mcc;
		}
		else
			__asm int 13;
	}
};