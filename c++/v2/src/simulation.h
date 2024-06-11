#include <SDL.h>
#include <iostream>
#include <vector>
#include <random>
#include "particle.h"

class Simulation {
public:
	Simulation();
	~Simulation();

	void main_loop();

private:

	float dt = 0.1f;

	void initialize_SDL();
	void initialize_sim();
	void draw_circle(SDL_Renderer *renderer, Particle *p);

	unsigned int width = 1024;
	unsigned int height = 768;

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	std::vector<Particle> particles;

};

