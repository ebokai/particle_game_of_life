#define SDL_MAIN_HANDLED

#include "pgol.h"

int main(int argc, char *argv[]){

	// SCREEN SIZE =====
	unsigned int width = 1600;
	unsigned int height = 900;

	// SIMULATION =====
	Framework framework(width, height);
	framework.initialize();
	framework.main_loop();

	return 0;
}
