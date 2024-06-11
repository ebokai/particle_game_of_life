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

	// particle number
	int n_groups = 5;
	int n_per_group = 300;
	int n_particles = n_groups * n_per_group;
	
	float particle_radius = 4.0f;
	float collide_radius = 5.0f;

	// simulation size
	int width = 1440;
	int height = 900;
	int margin = 180;

	int x_cells = (width + 2 * margin) / margin;
	int y_cells = (height + 2 * margin) / margin;

	// parameters
	float dt = 0.1f;
	float coef = 1.0f;
	float max_force = 100.0f;

	// methods
	void initialize_SDL();
	void initialize_sim(bool first = true);
	void draw_circle(SDL_Renderer *renderer, Particle *p);
	void build_spatial_grid();
	void interact();
	void collide(Particle &p1, Particle &p2);
	void attract(Particle &p1, Particle &p2);
	void handle_key_press(SDL_Event &event);
	float dbound(float dx, int lim);

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	std::vector<Particle> particles;
	std::vector<std::vector<std::vector<Particle>>> spatial_grid;
	std::vector<std::vector<float>> forces;

};

