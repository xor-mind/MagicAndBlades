#ifndef PARTICLE_H
#define PARTICLE_H

#include "SDL.h"
#include "SDL_Surface.h"
#include "Rectangle.h"
#include <vector>

struct Particle
{
	float x, y;
	float xVel, yVel;
	int color;
	Uint32 lifetime;
	Uint32 startTime;
	bool alive;
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

	virtual void Init( int x, int y ) { this->x = x; this->y = y; Init( particleCount ); }
	virtual void Init() { Init( particleCount ); }
	virtual void Init( int count )
	{
		particleCount = count;
		particle.resize( count );

		for (int i = 0; i < particleCount; i++)
		{
			AddParticle( particle[i] );
		}
	}
	virtual void Render(SDL_Surface* screen, UsefulMath::Rectangle& r) const = 0;
	virtual void Update() = 0;
	virtual unsigned int AddParticle( Particle& p )  = 0;
};


#endif