#ifndef GAME_H
#define GAME_H

#include "MaB_Types.h"

class Map;
struct Entity;

struct Game
{
	Entity* camera;
	Map* map;

	Game() : map( nullptr ) {}
	~Game() { }
};

#endif