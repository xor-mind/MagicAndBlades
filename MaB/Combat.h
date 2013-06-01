#ifndef COMBAT_H
#define COMBAT_H

#include "Entity.h"

class Combat 
{
public:
	Entity* target;
	Entity* e;
public:
	Combat( Entity* e) : e(e), target( nullptr )
	{
	
	}
	// K.I.S.S. keep it simple stupid
	void Logic() 
	{
		// a basic combat hierarchy of if/else statements

		if ( target )
		{
			// if injured but we think have a greater than 50% probabilty of killing them,
			// attack them!
			// else evade and heal up

			// try and attack! no need for cover or anything else :D
			if ( !e->attack->melee.Attack( target ) )
			{
				e->MoveToPlayer( target );
			}
		}
	}
};

#endif