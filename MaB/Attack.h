#ifndef ATTACK_H
#define ATTACK_H

#include "Entity.h"

class Attack 
{
public:
	Entity* target;
	Entity* e;
public:
	Attack( Entity* e) : e(e), target( nullptr )
	{
	
	}

	void Logic() 
	{
		if ( target )
			e->MoveToPlayer( target );
	}
};

#endif