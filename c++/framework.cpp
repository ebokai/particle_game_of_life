#include "framework.h"
#include <time.h>
#include <iostream>

Framework::Framework(int n_, unsigned int width_, unsigned int height_): n(n_), width(width_), height(height_){
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

Framework::~Framework(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Framework::main_loop(){
	SDL_Event event;
	while(!(event.type == SDL_QUIT)){
		SDL_Delay(0);
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT) return;
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);=
		for(int i = 0; i < n; i++){
			SDL_RenderDrawPoint(renderer, particles[i].x, particles[i].y);
		}
		SDL_RenderPresent(renderer);
	}
}

void Framework::initialize(){
	srand(time(NULL));
	for(int i = 0; i < n; i++){
		particles.push_back(Particle());
		particles[i].p_ID = i;
		particles[i].x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/width);
		particles[i].y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/height);
	}
}