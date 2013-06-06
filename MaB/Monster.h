#ifndef MONSTER_H
#define MONSTER_H

#include "Combat.h"

class Monster : public Entity
{
protected:
	Combat combat;
public:

	Monster(Game& g) : Entity(g), combat( this ) { NPC = true; }
	virtual ~Monster() {}

	void Logic()
	{
		combat.Logic();
		Entity::Logic();
	}

	// some properties of monsters


	void Immortal( bool b ) { immortal = b; }
	void InstaKill( bool b ) { instaKill = b; }

	bool immortal;
	bool instaKill;
};

#endif