#ifndef BLOOD_PARTICLES_01_H
#define BLOOD_PARTICLES_01_H

#include "particle.h"
#include "SDL_Gfx.h"

class BloodEmitter01 : public ParticleEmitter
{
	
public:
	BloodEmitter01()
	{
		particleCount = 200;
		x = 0;
		y = 0;
		speed = 3.f;
		colour = 0;
		lifetime = 500;
	}

	void Init( int x, int y ) override 
	{
		ParticleEmitter::Init( x, y );
	}
	void Render(SDL_Surface* screen) const override
	{
		SDL_LockSurface( screen );

		for (int i = 0; i < particleCount; i++)
		{
			if ( particle[i].x >= 0 && particle[i].x < screen->w )
				if ( particle[i].y >= 0 && particle[i].y < screen->h )
					SDL_Video::put_pixel32(screen, (int)particle[i].x, (int)particle[i].y, 0xff3333 );
		}

		SDL_UnlockSurface( screen );
	}
	void Update() override
	{
		for (int i = 0; i < particleCount; i++)
		{
			unsigned int timeLeft = SDL_GetTicks() - particle[i].startTime;
			if ( timeLeft > particle[i].lifetime)
			{
				AddParticle( particle[i] );
			}
			else
			{
				particle[i].x += particle[i].xVel;
				particle[i].y += particle[i].yVel;
				particle[i].yVel += .5f;
			}
		}
	}
	void AddParticle( Particle& p ) override
	{
			// reset the x, y coords
			p.x = (float)x;
			p.y = (float)y;

			p.xVel =  // between -2.5 and 2.5
				//( (rand() % int(speed * 10)) - (speed * 5)) / 8.0;
				-(rand()%100 + 100)/100.f;
			p.yVel =  -(rand() % int(speed * 10) / 5.0); // between -10 and 0

			p.color = 0xff3333;
			p.lifetime = lifetime; //rand() % lifetime + (lifetime >> 2);

			// restart the timer
			p.startTime = SDL_GetTicks();
	}
};

#endif