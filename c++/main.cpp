#define SDL_MAIN_HANDLED

#include "pgol.h"

int main(int argc, char *argv[]){

	// SCREEN SIZE =====
	unsigned int width = 1600;
	unsigned int height = 900;

	// NUMBER OF PARTICLES =====
	int n_target = 10000; // target number of particles
	int n_groups = 10; // number of groups
	int pp_group = n_target / n_groups; // number of particles per group
	int n = n_groups * pp_group; // actual number of particles

	cout << n << endl;

	// SIMULATION =====
	Framework framework(n, width, height);
	framework.initialize(n_groups, pp_group);
	framework.main_loop();

	return 0;
}
