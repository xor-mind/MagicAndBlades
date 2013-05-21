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
		Rect r = FoV;
		r.left  -= cam.left; r.top -= cam.top;
		r.right -= cam.left; r.bottom -= cam.top;
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
	Dialog * dialog;

	Player( Game& g) : Entity(g), health(100) 
	{
		pos = vel = Vector(0,0);
		dim = Vector(32,32);
		fovDim = Vector(100, 100);
		dialog = nullptr;
	}
	

	void KeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
	{
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
		switch ( sym )
		{
			case SDLK_d: vel.x = 0; break;
			case SDLK_a: vel.x = 0; break;
			case SDLK_w: vel.y = 0; break;
			case SDLK_s: vel.y = 0; break;
			default: break;
		}
	}

	void Logic()
	{
		Entity::Logic();
		if ( dialog ) dialog->Update(); 
	}

	void Render( SDL_Surface* dest ) override
	{
		Rect cr = g.camera->Rectangle();
		if ( cr.Intersect( Rectangle() ) )
		{
			if ( dialog )
			{	
				dialog->UpdateRect( cr );
				dialog->Render( dest, cr );
			}
		}
		Entity::Render( dest );
	}

	void MultipleChoiceEvent(MultipleChoice* mc) override
	{
		// create dialog and send answer back to deamon
		if ( dialog )
			__asm int 13; // WTF!?

		dialog = new InteractiveDialog();
		dialog->msgs.push_back( mc->question );
		((InteractiveDialog*)dialog)->options = mc->answers;
		dialog->CalcLines();
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
	void MultipleChoiceEvent(MultipleChoice* mc)
	{
		if ( mc == &this->mc )
		{
			if ( mc->selectedAnswer == 0 )
			{
				// tell player, "You selfish son of a bitch, you're dead!"
				// attack player
				__asm nop;
			}
			else if ( mc->selectedAnswer == 1 )
			{
				// tell player, "You're loyalty is worthy of the gods themselves."
				// give player treasure
				// + ally player
				__asm nop;
			}
		}
	}
};