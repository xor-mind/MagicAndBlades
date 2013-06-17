#include "SDL.h"
#include "SDL_Surface.h"
#include <vector>

struct Particle
{
	float x;
	float y;
	float xVel;
	float yVel;
	int color;
	Uint32 lifetime;

	Uint32 startTime;
};

static SDL_Surface *particleColour[10];

void initialise_particles()
{
	particleColour[0] = Surface::PngLoad("images/black.png", true);
	particleColour[1] = Surface::PngLoad("images/blue.png", true);
	particleColour[2] = Surface::PngLoad("images/green.png", true);
	particleColour[3] = Surface::PngLoad("images/orange.png", true);
	particleColour[4] = Surface::PngLoad("images/pink.png", true);
	particleColour[5] = Surface::PngLoad("images/purple.png", true);
	particleColour[6] = Surface::PngLoad("images/red.png", true);
	particleColour[7] = Surface::PngLoad("images/teal.png", true);
	particleColour[8] = Surface::PngLoad("images/white.png", true);
	particleColour[9] = Surface::PngLoad("images/yellow.png", true);
}

class ParticleEmitter
{
public:
	ParticleEmitter();
	void set_particle_count(int countArg)
	{
		particleCount = countArg;
		particle.resize(particleCount);

		for (i = 0; i < particleCount; i++)
		{
			particle[i].startTime = SDL_GetTicks();
			particle[i].color = 1;
		}
	}
	void set_xy(int xArg, int yArg)
	{
		x = xArg;
		y = yArg;
	}
	void set_speed(float speedArg)
	{
		speed = speedArg;
	}
	void set_colour(int colourArg)
	{
		colour = colourArg;
	}
	void IncreaseColour()
	{
		colour++;
		if ( colour == 10 )
			colour = 0;
	}
	void DecreaseColour()
	{
		colour--;
		if ( colour == -1 )
			colour = 9;
	}
	void set_lifetime(int lifetimeArg)
	{
		lifetime = lifetimeArg;
		for (i = 0; i < particleCount; i++)
		{
			particle[i].lifetime = rand() % lifetime + lifetime / 4;
		}
	}
	void draw(SDL_Surface* screen)
	{
		for (i = 0; i < particleCount; i++)
		{
			Surface::OnDraw(screen, particleColour[particle[i].color], 300 + particle[i].x, 300 + particle[i].y );
		}
	}

	void update();

private:
	std::vector <Particle> particle;

	int i;
	int particleCount;
	int x;
	int y;
	float speed;
	int colour;
	int lifetime;
};

ParticleEmitter::ParticleEmitter()
{
	particleCount=100;
	x = 0;
	y = 0;
	speed = 0.01;
	colour = 0;
	lifetime = 1000;
}

void ParticleEmitter::update()
{
	for (i = 0; i < particleCount; i++)
	{
		unsigned int timeLeft = SDL_GetTicks() - particle[i].startTime;
		if ( timeLeft > particle[i].lifetime)
		{
			// reset the x, y coords
			particle[i].x = x;
			particle[i].y = y;

			// randomly pick negetive or positive velocity
			particle[i].xVel = rand() % 100;
			particle[i].yVel = rand() % 100;

			if (rand() % 2)
				particle[i].xVel = particle[i].xVel * -1;

			if (rand() % 2)
				particle[i].yVel = particle[i].yVel * -1;

			// restart the timer
			particle[i].startTime = SDL_GetTicks();
			particle[i].color = 1;
		}
		else
		{
			if ( timeLeft/(float)particle[i].lifetime > 0.5f )
				particle[i].color = 5;
			particle[i].x += particle[i].xVel * speed;
			particle[i].y += particle[i].yVel * speed;
		}
	}
}
