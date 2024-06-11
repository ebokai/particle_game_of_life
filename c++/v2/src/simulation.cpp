#include "simulation.h"

// constructor
Simulation::Simulation() {
	initialize_SDL();
	initialize_sim();
	
}

// destructor
Simulation::~Simulation() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

// main loop
void Simulation::main_loop() {

	SDL_Event event;

	while(!(event.type == SDL_QUIT)) {

		Uint64 start = SDL_GetPerformanceCounter();

		SDL_Delay(0);
		SDL_PollEvent(&event);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		for (auto& p : particles) {
			p.update_pos(dt);
			draw_circle(renderer, &p);
		}

		SDL_RenderPresent(renderer);

		Uint64 end = SDL_GetPerformanceCounter();
		float secs = (end - start) / (float) SDL_GetPerformanceFrequency();
		float fps = (1.0f) / secs;
		std::cout << fps << std::endl;
	}


}

void Simulation::initialize_SDL() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	return;
}


void Simulation::initialize_sim() {

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis_x(0.0f, static_cast<float>(width));
	std::uniform_real_distribution<float> dis_y(0.0f, static_cast<float>(height));
	std::uniform_real_distribution<float> dis_v(-2.0f, 2.0f);
	std::uniform_int_distribution<int> dis_c(0, 255);

	for (unsigned int i = 0; i < 20; i++) {
		float x = dis_x(gen);
		float y = dis_y(gen);
		float vx = dis_v(gen);
		float vy = dis_v(gen);

		int R = dis_c(gen);
		int G = dis_c(gen);
		int B = dis_c(gen);

		Particle p;
		p.set_position(x, y);
		p.set_velocity(vx, vy);
		p.set_color(R,G,B);
		p.label = i;
		particles.push_back(p);

	}
}

void Simulation::draw_circle(SDL_Renderer *renderer, Particle *p) {

	SDL_SetRenderDrawColor(renderer, p->R, p->G, p->B, 255);

	int icx = static_cast<int>(p->x);
	int icy = static_cast<int>(p->y);
	int ird = static_cast<int>(p->radius);

	int x = ird - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (ird * 2);

	while (x >= y) {

		SDL_RenderDrawLine(renderer, (icx - x), (icy + y), (icx + x), (icy + y));
        SDL_RenderDrawLine(renderer, (icx - y), (icy + x), (icx + y), (icy + x));
        SDL_RenderDrawLine(renderer, (icx - x), (icy - y), (icx + x), (icy - y));
        SDL_RenderDrawLine(renderer, (icx - y), (icy - x), (icx + y), (icy - x));

        if (err <= 0) {
        	++y;
        	err += dy;
        	dy += 2;
        }

        if (err > 0) {
        	--x;
        	dx += 2;
        	err += dx - (ird * 2);
        }

	}

}