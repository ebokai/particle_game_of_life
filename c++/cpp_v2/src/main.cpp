#define SDL_MAIN_HANDLED

#include "simulation.h"

int main(int argc, char *argv[]) {

	Simulation simulation;
	simulation.main_loop();

	return 0;
}