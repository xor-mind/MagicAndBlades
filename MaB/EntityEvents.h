#ifndef ENTITYEVENTS_H
#define ENTITYEVENTS_H

#include "MaB_Types.h"

enum EntityEventType { ATTACK, MULTIPLE_CHOICE, ENTER_FOV };

struct EntityEvent {
	EntityEventType  type;
};
typedef std::list<EntityEvent*> EntityEventList;

class MultipleChoice;

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

class MultipleChoice : public EntityEvent
{
public:
	Str question;
	Strs answers;
	int selectedAnswer;
	std::list< EntityEventManager* > playersQuestioned;
	EntityEventManager* questionner;
public:
	MultipleChoice() { type = MULTIPLE_CHOICE; }
	MultipleChoice(EntityEventManager* questionner, Str& question, const Strs& answers)
		: selectedAnswer(-1)
	{
		type = MULTIPLE_CHOICE;
		this->question = question; this->answers = answers;
		this->questionner = questionner;
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
			player->entityEvents.push_back(this); // adds a dialog to the player render list
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