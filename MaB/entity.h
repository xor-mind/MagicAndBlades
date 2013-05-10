#include "2DMath.h"
#include "types.h"
#include <algorithm>
#include "SDL.h"
#include "SDL_surface.h"
#include <list>
#include <boost/assign/std/vector.hpp> // for 'operator+=()'
using namespace boost::assign; // bring 'operator+=()' into scope

class Player;
struct Entity;
class MultipleChoice;

typedef std::vector<Player*> vPlayers; 
typedef std::vector<Player*> lPlayers; 
typedef std::vector<Entity*> vEntities; 

enum EE { ATTACK, MULTIPLE_CHOICE, ENTER_FOV };

struct EntityEvent {
	int  type;
};
typedef std::list<EntityEvent*> lEntityEvents;

class EntityEvents
{
public:
	virtual ~EntityEvents() {}
	lEntityEvents entityEvents;

	virtual void OnEvent(EntityEvent* Event) 
	{
		switch(Event->type) 
		{
			case MULTIPLE_CHOICE: 
			{
				MultipleChoiceEvent((MultipleChoice*)Event);		
			} break;
		}	
	}

	virtual void MultipleChoiceEvent(MultipleChoice* mc)
	{
	}
};

struct Entity : public EntityEvents
{
	math::Rectangle FoV;
	Vector fovDim;
	SDL_Surface * model;
	Entity() : model(nullptr) {}
	virtual ~Entity() {}
	Vector pos, dim, vel;
	math::Rectangle Rect() { return math::Rectangle( pos, dim ); }
	
	void Logic()
	{
		// if dialogDisplay
		// display

		//static auto startTime = SDL_GetTicks();
		//if ( SDL_GetTicks() - startTime > 150 )
		{
			pos += vel;
			FoV.left = pos.x + dim.x/2 - fovDim.x;
			FoV.right = pos.x + dim.x/2 + fovDim.x;
			FoV.top = pos.y + dim.y/2 - fovDim.y;
			FoV.bottom = pos.y + dim.y/2 + fovDim.x;
			//fov.
			//startTime = SDL_GetTicks();
		}
	}
	void PushEvent(EntityEvent* Event) 
	{
		entityEvents.push_back( Event );
	}
	virtual bool Init(SDL_Surface* s)
	{
		model = s;
		return true;
	}
};

struct EnterFov : public EntityEvent
{
	Player* p;
	EnterFov()
	{
		type = ENTER_FOV;
	}
};

class MultipleChoice : public EntityEvent
{
public:
	Str question;
	Strs answers;
	int selectedAnswer;
	vEntities playersQuestioned;
	Entity* questionner;
public:
	MultipleChoice() {}
	MultipleChoice(Entity* questionner, Str& question, const Strs& answers)
		: selectedAnswer(-1)
	{
		type = MULTIPLE_CHOICE;
		this->question = question; this->answers = answers;
		this->questionner = questionner;
	}
	void Init(Entity* questionner, Str& question, const Strs& answers)
	{
		this->questionner = questionner; this->question = question; this->answers = answers;
	}
	
	void AskPlayer(Entity* player)
	{
		// if ( player == playerCharacter )
		// ...

		// make sure not to spam the player with requests for a question
		if ( std::find( playersQuestioned.begin(), playersQuestioned.end(), player) != 
			playersQuestioned.end() )
		{
			player->PushEvent(this); // adds a dialog to the player render list
			playersQuestioned.push_back(player);
		}
	}

	//void ClosedQuestion()
	//{
	//}
};

class Monster : public Entity
{
public:
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
	

	Player() : health(100) 
	{
		pos = vel = Vector(0,0);
		dim = Vector(32,32);
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

	void Render( SDL_Surface* s )
	{
		Surface::OnDraw( s, model, (int)pos.x, (int)pos.y );
	}

	void MultipleChoiceEvent(MultipleChoice* mc) override
	{
		// create dialog and send answer back to deamon
	}
};

class AggGrool : public Monster
{
public:
	int health;
	MultipleChoice mc; 
	AggGrool() : health(100) 
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