#include <SDL.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include "particle.h"



class Simulation {
public:
	Simulation();
	~Simulation();

	void main_loop();

private:

	int n_particles = 1000;

	int width = 1024;
	int height = 768;
	int margin = 64;

	int x_cells = (width + 2 * margin) / margin;
	int y_cells = (height + 2 * margin) / margin;



	float dt = 0.1f;

	void initialize_SDL();
	void initialize_sim();
	void draw_circle(SDL_Renderer *renderer, Particle *p);
	void build_spatial_grid();
	void interact();
	void collide(Particle &p1, Particle &p2);
	void attract(Particle &p1, Particle &p2);

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	std::vector<Particle> particles;
	std::vector<std::vector<std::vector<Particle>>> spatial_grid;

};

