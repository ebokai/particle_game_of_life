#include <time.h>
#include "pgol.h"

// CONSTRUCTOR - INITIALIZE SDL
Framework::Framework(unsigned int width_, unsigned int height_): width(width_), height(height_){
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

// DESTRUCTOR
Framework::~Framework(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

// MAIN LOOP 
void Framework::main_loop(){
	SDL_Event event;

	unsigned int frame = 0;
	float tot_fps = 0;

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = width;
	rect.h = height;

	while(!(event.type == SDL_QUIT)){

		Uint64 start = SDL_GetPerformanceCounter();

		// SDL INITIALIZE FRAME =====
		SDL_Delay(0);
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT) return;


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, trail_alpha);
		SDL_RenderFillRect(renderer, &rect);
		//SDL_RenderClear(renderer);

		// SIMULATION UPDATES =====
		interact();
		friction();
		update_and_bound();
		make_hash_table();		

		// DRAW PARTICLES =====
		for(unsigned int i = 0; i < n; i++){
			SDL_SetRenderDrawColor(renderer, particles[i].R, particles[i].G, particles[i].B, particles[i].A);

			for (int x = particles[i].x-radius; x <= particles[i].x+radius; x++){
				for (int y = particles[i].y-radius; y <= particles[i].y+radius; y++){

					float X2 = (particles[i].x*particles[i].x) - 2 * (x*particles[i].x) + (x*x);
					float Y2 = (particles[i].y*particles[i].y) - 2 * (y*particles[i].y) + (y*y);

					if((X2 + Y2) <= (radius*radius)){
						SDL_RenderDrawPoint(renderer, x, y);
					}
				}
			}
		}
		SDL_RenderPresent(renderer);

		// PERFORMANCE =====
		frame++;
		Uint64 end = SDL_GetPerformanceCounter();
		float secs = (end - start) / (float) SDL_GetPerformanceFrequency();
		float fps = (1.0f) / secs;
		tot_fps += fps;
		cout << tot_fps/frame << endl;
	}
}

// INITIALIZE CLASS ATTRIBUTES
void Framework::initialize(){
	srand(time(NULL));


	// simulation parameters [random]
	float dRepel = 10 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX/50);
	float dRepelSq = dRepel * dRepel;
	float repel_strength = 400 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX/1000);

	float dForce = dRepel + static_cast <float> (rand()) / static_cast <float> (RAND_MAX/50);
	float dForceSq = dForce * dForce;
	int force_strength = 20 + (rand()) / (RAND_MAX/80);

	float friction_strength = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/20);

	// INITIALIZE PARTICLES 
	for (unsigned int i = 0; i < n_groups; i++){

		// group color
		unsigned int R = rand() / (RAND_MAX/255);
		unsigned int G = rand() / (RAND_MAX/255);
		unsigned int B = rand() / (RAND_MAX/255);
		unsigned int A = 255; 

		
		for (unsigned int j = 0; j < pp_group; j++){
			unsigned int k = i * pp_group + j;
			particles.push_back(Particle());
			particles[k].p_ID = k;
			particles[k].x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/width);
			particles[k].y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/height);
			particles[k].vx = -50 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX/100);
			particles[k].vy = -50 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX/100);
			particles[k].R = R;
			particles[k].G = G;
			particles[k].B = B;
			particles[k].A = A;
		}
	}

	// POPULATE FORCE ARRAY
	vector<vector<int>> new_forces(n, vector<int>(n));

	for (unsigned int i = 0; i < n_groups; i++){
		for(unsigned int j = 0; j < n_groups; j++){
			int force = -force_strength/2 + rand() / (RAND_MAX/force_strength);

			for(unsigned int k = i * pp_group; k < (i+1) * pp_group; k++){
				for(unsigned int l = j * pp_group; l < (j+1) * pp_group; l++){
					new_forces[k][l] = force;

					if (k == l){
						new_forces[k][l] = 0;
					}
				}
			}
		}
	}

	make_hash_table();
	forces = new_forces;

}
