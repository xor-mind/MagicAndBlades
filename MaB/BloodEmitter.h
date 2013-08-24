#ifndef BLOOD_PARTICLES_01_H
#define BLOOD_PARTICLES_01_H

#include "particle.h"
#include "SDL_Gfx.h"


typedef unsigned int uint;

class BloodEmitter01 : public ParticleEmitter
{
	uint emitterLife;
	uint emitterStartTime;
	bool emitterDead;
public:
	BloodEmitter01()
	{
		emitterLife = 500;
		particleCount = 100;
		x = 0;
		y = 0;
		speed = 3.f;
		colour = 0;
		lifetime = 500;
		emitterDead = true;
	}
	virtual ~BloodEmitter01() {}

	void Init( int x, int y ) override 
	{
		ParticleEmitter::Init( x, y );
		emitterStartTime = SDL_GetTicks();
		emitterDead = false;
	}
	void Render(SDL_Surface* screen, UsefulMath::Rectangle& r) const override
	{
		if ( emitterDead )
			return;
		SDL_LockSurface( screen );

		int size = particle.size();
		for (int i = 0; i < size; i++)
		{
			if ( particle[i].alive )
				if ( particle[i].x >= 0 && particle[i].x < screen->w )
					if ( particle[i].y >= 0 && particle[i].y < screen->h )
					{
						SDL_Video::put_pixel32(screen, (int)(particle[i].x - r.left), (int)(particle[i].y - r.top), 0xff3333 );
						SDL_Video::put_pixel32(screen, (int)(particle[i].x - 1 - r.left), (int)(particle[i].y - r.top), 0xff3333 );
						SDL_Video::put_pixel32(screen, (int)(particle[i].x - r.left), (int)(particle[i].y + 1 - r.top), 0xff3333 );
						SDL_Video::put_pixel32(screen, (int)(particle[i].x - 1 - r.left), (int)(particle[i].y + 1 - r.top), 0xff3333 );
					}
		}

		SDL_UnlockSurface( screen );
	}
    void Update() override
	{
		unsigned int t = SDL_GetTicks();
		int size = particle.size();
		for (int i = 0; i < size; i++)
		{
			if ( t - emitterStartTime > emitterLife ) {
				particle[i].alive = false;
				emitterDead = true;
				break;
			}
			else
				if ( t - particle[i].startTime > particle[i].lifetime )
				{
					//particle[i].alive = false;
					t = AddParticle( particle[i] );
				}
				else
				//if ( timeLeft < particle[i].lifetime)
				{
					particle[i].x += particle[i].xVel;
					particle[i].y += particle[i].yVel;
					particle[i].yVel += .5f;
				}
			//else
			//	particle[i].alive = false;
		}
	}
	unsigned int AddParticle( Particle& p ) override
	{
		// reset the x, y coords
		p.x = (float)x;
		p.y = (float)y;

		// blood to the left
		p.xVel = -(rand()%100 + 100)/50.f;
		p.yVel =  (float)( -(rand() % int(speed * 10) / 5.0) ); // between -10 and 0

		p.color = 0xff3333;
		p.lifetime = rand() % lifetime + (lifetime >> 2);
		p.alive = true;
		// restart the timer
		return p.startTime = SDL_GetTicks();
	}
};

class BloodEmitter02 : public BloodEmitter01
{
public:
	unsigned int AddParticle( Particle& p ) override
	{
		// reset the x, y coords
		p.x = (float)x;
		p.y = (float)y;

		p.xVel =  // blood sprays evenly between -2.5 and 2.5
			(float)(( (rand() % int(speed * 10)) - (speed * 5)) / 8.0);

		p.yVel =  (float)( -(rand() % int(speed * 10) / 5.0) ); // between -10 and 0

		p.color = 0xff3333;
		p.lifetime = rand() % lifetime + (lifetime >> 2);
		p.alive = true;
		// restart the timer
		return p.startTime = SDL_GetTicks();
	}
};

class BloodEmitter : public ParticleEmitter
{
	ParticleEmitter* be01, *be02, *currentEmitter;

public:
	BloodEmitter() 
	{
		be01 = new BloodEmitter01();
		be02 = new BloodEmitter02();
		currentEmitter = be01;
	}
	~BloodEmitter()
	{
		delete be01; delete be02;
	}
	void Init( int x, int y )
	{
		if ( rand()%2 == 1 )
			currentEmitter = be01;
		else
			currentEmitter = be02;

		currentEmitter->Init( x, y );
	}

	void Update()
	{
		currentEmitter->Update();
	}

	void Render(SDL_Surface* screen, UsefulMath::Rectangle& r) const
	{
		currentEmitter->Render( screen, r );
	}
	unsigned int AddParticle( Particle& p ) override { return 0; }
};

#endif