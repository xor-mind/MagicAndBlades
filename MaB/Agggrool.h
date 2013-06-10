#ifndef AGGGROOL_H
#define AGGGROOL_H

#include "Monster.h"

class AggGrool : public Monster
{
public:
	
	int health;
	MultipleChoice mc; 
	AggGrool() : health(100) 
	{
		
		pos = vel = Vector(0,0);
		speed = 1.f;
		dim = Vector(32,32);
		fovDim = Vector(100, 100);
	}

	bool Init(SDL_Surface* model, SDL_Surface* healthBar ) override
	{
		Strength( 100 );
		Immortal(true);
		InstaKill(true);
		//Aggro(new AttackSecond());

		Str question = "Do you believe in fate?";
		Strs answers;
		answers += "Yes.",
				   "No.";
		mc.Init(this, question, answers);

		return Entity::Init( model, healthBar );
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
				combat.target = (Entity*)mcc->to;
			}
		}
		else
			__asm int 13;
	}
};

#endif