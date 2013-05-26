#ifndef AGGGROOL_H
#define AGGGROOL_H

#include "Entity.h"

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

		Str question = "Do you believe in fate?";
		Strs answers;
		answers += "Yes.",
				   "No.";
		mc.Init(this, question, answers);

		return Entity::Init(s);
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
		int s = mcc->SelectedAnswer();
		if (  s >= 0 )
		{
			// do something
			mcc->isComplete = true;
			
			if ( s == 1 )
			{
				// attack the player
				//entityEvents.push_back(
			}
		}
		else
			__asm int 13;
	}
};

#endif