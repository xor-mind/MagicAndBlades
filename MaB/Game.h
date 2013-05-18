#ifndef GAME_H
#define GAME_H

#include "MaB_Types.h"

class Map;

struct Game
{
	Rect camera;
	Map* map;

	Game() : map( nullptr ) {}
	~Game();
};

#endif