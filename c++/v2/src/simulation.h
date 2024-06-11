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

	int width = 1024;
	int height = 768;
	int margin = 32;
	float dt = 0.1f;

	void initialize_SDL();
	void initialize_sim();
	void draw_circle(SDL_Renderer *renderer, Particle *p);

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	std::vector<Particle> particles;

};

