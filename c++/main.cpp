#define SDL_MAIN_HANDLED

#include "framework.h"

int main(int argc, char *argv[]){

	int n = 1000;
	unsigned int width = 1600;
	unsigned int height = 900;

	Framework framework(n, width, height);
	framework.initialize();
	framework.main_loop();

	return 0;
}
