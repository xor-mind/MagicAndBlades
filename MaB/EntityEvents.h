#ifndef ENTITYEVENTS_H
#define ENTITYEVENTS_H

#include "MaB_Types.h"
#include "Dialog.h"

enum EntityEventType { ATTACK, MULTIPLE_CHOICE_CHUNK, ENTER_FOV };

struct EntityEvent {
	bool isComplete;
	EntityEvent() : isComplete( false ) { }
	EntityEventType  type;
	bool Complete() { return isComplete; } 
	virtual void KeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) {}
	virtual void KeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) {}
	virtual void LButtonDown(int mX, int mY) {}
	virtual void RButtonDown(int mX, int mY) {}
	virtual void Logic() {}
	virtual void Render( SDL_Surface* dest, const Rect& cr ) {}
};
typedef std::list<EntityEvent*> EntityEventList;
typedef std::vector<EntityEvent*> EntityEvents;

class MultipleChoiceChunk;
class Attack;

class EntityEventManager
{
public:
	EntityEventList entityEvents;
public:
	virtual ~EntityEventManager() {}

	virtual void ProcessEvent() 
	{
		if ( entityEvents.empty() )
			return;

		EntityEvent* Event = entityEvents.front();
		entityEvents.pop_front();

		switch(Event->type) 
		{
			case MULTIPLE_CHOICE_CHUNK: 
			{
				MultipleChoiceChunkEvent((MultipleChoiceChunk*)Event);		
			} break;
		}	
	}

	virtual void MultipleChoiceChunkEvent(MultipleChoiceChunk* mcc) {}
};

class Attack : public EntityEvent
{
public:
	Attack( )
	{
		type = ATTACK;
	}
};

class MultipleChoiceChunk: public EntityEvent
{
	Dialog* dialog;
	EntityEventManager* from;
public:
	MultipleChoiceChunk(EntityEventManager* from, Str& question, Strs& answers) 
		: from(from) 
	{
		type = MULTIPLE_CHOICE_CHUNK;
		dialog = new Dialog();
		dialog->msgs.push_back( question );
		dialog->options = answers;
		dialog->CalcLines();
	}
	~MultipleChoiceChunk()
	{
		delete dialog;
	}
	
	void Logic() override
	{
		if ( dialog ) 
		{ 
			if ( dialog->dialogRead ) {
				from->entityEvents.push_back( this );
			}
			else
				dialog->Update(); 
		}
	}
	void KeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) override { dialog->KeyDown( sym, mod, unicode ); }
	void LButtonDown(int mX, int mY) override { dialog->LButtonDown( mX, mY ); }
	void RButtonDown(int mX, int mY) override { dialog->RButtonDown( mX, mY ); }
	void Render( SDL_Surface* dest, const Rect& cr ) override { dialog->Render( dest, cr ); }
	int SelectedAnswer() { return dialog->currentSelection; }
};

class MultipleChoice
{
public:
	Str question;
	Strs answers;
	
	std::list< EntityEventManager* > playersQuestioned;
	EntityEventManager* questionner;
public:
	MultipleChoice() {}
	MultipleChoice(EntityEventManager* questionner, Str& question, const Strs& answers)
	{
		this->questionner = questionner; this->question = question; this->answers = answers;
	}
	
	void Init(EntityEventManager* questionner, Str& question, const Strs& answers)
	{
		this->questionner = questionner; this->question = question; this->answers = answers;
	}
	void AskPlayer(EntityEventManager* player)
	{
		// if ( player == playerCharacter )
		// ...

		// make sure not to spam the player with requests for a question
		if ( std::find( playersQuestioned.begin(), playersQuestioned.end(), player) == 
			playersQuestioned.end() )
		{
			MultipleChoiceChunk* mcc 
				= new MultipleChoiceChunk(questionner, question, answers);
			player->entityEvents.push_back(mcc); // adds a dialog to the player render list
			playersQuestioned.push_back(player);
		}
	}

	//void ClosedQuestion()
	//{
	//}
};

//struct EnterFov : public EntityEvent
//{
//	Player* p;
//	EnterFov()
//	{
//		type = ENTER_FOV;
//	}
//};


#endif