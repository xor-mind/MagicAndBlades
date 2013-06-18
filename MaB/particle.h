#ifndef particle_h
#define particle_h

#include "SDL.h"
#include "SDL_Surface.h"
#include <vector>

struct Particle
{
	float x, y;
	float xVel, yVel;
	int color;
	Uint32 lifetime;
	Uint32 startTime;
};

class ParticleEmitter
{
protected:
	std::vector <Particle> particle;

	int particleCount;
	int x, y;
	float speed;
	int colour;
	int lifetime;

public:
	virtual ~ParticleEmitter() {}

	virtual void Init() { Init( particleCount ); }
	virtual void Init( int count )
	{
		particleCount = count;
		particle.resize( count );

		for (int i = 0; i < particleCount; i++)
		{
			particle[i].startTime = SDL_GetTicks();
			particle[i].color = 1;
			particle[i].lifetime = rand() % lifetime + lifetime / 4;
		}
	}
	virtual void Render(SDL_Surface* screen) = 0;
	virtual void Update() = 0;
};

#endif