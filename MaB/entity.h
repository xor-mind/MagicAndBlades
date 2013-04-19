#include "2DMath.h"
#include "types.h"
#include <algorithm>
#include "SDL.h"
#include "SDL_surface.h"
#include <list>
#include <boost/assign/std/vector.hpp> // for 'operator+=()'
using namespace boost::assign; // bring 'operator+=()' into scope

class Player;
class GameEvents;
struct Entity;
class MultipleChoice;

typedef std::vector<Player*> vPlayers; 
typedef std::vector<Player*> lPlayers; 
typedef std::list<GameEvents> gameEvents;
typedef std::vector<Entity*> ePlayers; 

enum GE { ATTACK, MULTIPLE_CHOICE };

struct GameEvent {
	int  type;
};

class GameEvents
{
	virtual void OnEvent(GameEvent* Event) 
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

struct Entity : public GameEvents
{
	Vector pos, dim, vel;
	math::Rectangle Rect() { return math::Rectangle( pos, dim ); }
	void PushEvent(GameEvent* Event) {}
};

class MultipleChoice : public GameEvent
{
	Str question;
	Strs answers;
	int selectedAnswer;
	ePlayers playersQuestioned;
	Entity* questionner;
public:
	MultipleChoice(Entity* questionner, Str& question, const Strs& answers)
		: selectedAnswer(-1)
	{
		type = MULTIPLE_CHOICE;
		this->question = question; this->answers = answers;
		this->questionner = questionner;
	}
	void Init(Str& question, const Strs& answers)
	{
		this->question = question; this->answers = answers;
	}
	
	void AskPlayer(Entity* e)
	{
		// make sure not to spam the player with requests for a question
		if ( std::find( playersQuestioned.begin(), playersQuestioned.end(), e) != 
			playersQuestioned.end() )
		{
			e->PushEvent(this); // adds a dialog to the player render list
			playersQuestioned.push_back(e);
		}
	}

	void ClosedQuestion()
	{
	}
};





class Monster : public Entity
{
public:
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
	SDL_Surface * model;

	Player() : health(100) 
	{
		pos = vel = Vector(0,0);
		dim = Vector(32,32);
	}
	
	bool Init()
	{
		model = Surface::BmpLoad("./art/avatar01.bmp");
		return true;
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
		//static auto startTime = SDL_GetTicks();
		//if ( SDL_GetTicks() - startTime > 150 )
		{
			pos += vel;
			//startTime = SDL_GetTicks();
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
	MultipleChoice mc; 

	void Init()
	{
		Immortal(true);
		InstaKill(true);
		//Aggro(new AttackSecond());

		Str question = "If your first born son became ill and you were an unmarried old man whose girfriend refused to have him around, who would you choose?";
		Strs answers;
		answers += "I have to live my own life, my son must deal with the world on his own terms.",
				   "No matter what his well being comes first.";
		mc.Init(question, answers);
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
};