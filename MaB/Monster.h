#ifndef MONSTER_H
#define MONSTER_H

#include "Attack.h"

class Monster : public Entity
{
protected:
	Attack attack;
public:

	Monster(Game& g) : Entity(g), attack( this ) {}
	virtual ~Monster() {}

	void Logic()
	{
		attack.Logic();
		Entity::Logic();
	}

	// some properties of monsters


	void Immortal( bool b ) { immortal = b; }
	void InstaKill( bool b ) { instaKill = b; }

	bool immortal;
	bool instaKill;
};

#endif