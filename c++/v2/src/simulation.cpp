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

		build_spatial_grid();

		interact();

		for (auto &p : particles) {
			p.update_velocity(dt);
			p.bound_x(width, margin);
			p.bound_y(height, margin);
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

	// initialize RNG
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis_x(0.0f, static_cast<float>(width));
	std::uniform_real_distribution<float> dis_y(0.0f, static_cast<float>(height));
	std::uniform_real_distribution<float> dis_v(-5.0f, 5.0f);
	std::uniform_int_distribution<int> dis_c(0, 255);
	std::uniform_int_distribution<int> dis_s(0,2);

	// initialize spatial grid
	spatial_grid.resize(x_cells);
	for (int i = 0; i < x_cells; i++) {
		spatial_grid[i].resize(y_cells);
		for (int j = 0; j < y_cells; j++) {
			spatial_grid[i][j].resize(0);
		}
	}

	// initialize particles
	for (unsigned int i = 0; i < n_particles; i++) {
		float x = dis_x(gen);
		float y = dis_y(gen);
		float vx = 0; // dis_v(gen);
		float vy = 0; // dis_v(gen);

		// int R = dis_c(gen);
		// int G = dis_c(gen);
		// int B = dis_c(gen);

		Particle p;
		p.set_position(x, y, margin);
		p.set_velocity(vx, vy);
		
		p.label = i;
		p.sign = -1 + 2 * dis_s(gen);

		if (p.sign < 0) {
			p.set_color(255, 0, 0);
		} else {
			p.set_color(0, 0, 255);	
		}
		
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


void Simulation::build_spatial_grid() {
	for (int i = 0; i < x_cells; i++) {
		for (int j = 0; j < y_cells; j++) {
			spatial_grid[i][j].clear();
		}
	}

	for (auto& p : particles) {
		// p.set_color(0,255,0);
		p.new_ax = 0;
		p.new_ay = 0;
		p.update_pos(dt, width, height, margin);
		p.bound_x(width, margin);
		p.bound_y(height, margin);
		spatial_grid[p.cell_x][p.cell_y].push_back(p);	
	}
}


void Simulation::interact() {

	for (int i = 0; i < x_cells; i++) {
		for (int j = 0; j < y_cells; j++) {

			std::vector<Particle> &cell_one = spatial_grid[i][j];
			
			for (int di = -1; di <= 1; di++) {
				for (int dj = -1; dj <= 1; dj++) {

					int ni = (i + di + x_cells) % x_cells;
					int nj = (j + dj + y_cells) % y_cells;

					std::vector<Particle> &cell_two = spatial_grid[ni][nj];

					for (auto& p1 : cell_one) {

						
						
						for (auto& p2 : cell_two) {
							if (p1.label == p2.label) {continue;}

							collide(p1, p2);
							attract(p1, p2);

							
							particles[p2.label] = p2;
						}

						particles[p1.label] = p1;
						
					}
				}
			}
		}
	}
}

void Simulation::collide(Particle &p1, Particle &p2) {

	float coef = 0.95;
	float r = 2 * p1.radius;
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	float dr = dx * dx + dy * dy;
	
	if ((dr < r * r) & (dr > 0)) {
		// p1.set_color(255, 0, 0);
		// p2.set_color(255, 0, 0);

		float sdr = sqrt(dr);

		float vx1 = p1.vx;
		float vx2 = p2.vx;
		float vy1 = p1.vy;
		float vy2 = p2.vy;

		float ux = dx / sdr;
		float uy = dy / sdr;

		float mx = (p1.x + p2.x)/2.0f;
		float my = (p1.y + p2.y)/2.0f;

		float new_x1 = mx - p1.radius * ux;
		float new_y1 = my - p1.radius * uy;
		float new_x2 = mx + p2.radius * ux;
		float new_y2 = my + p2.radius * uy;

		float dzdz = (new_x1 - new_x2) * (new_x1 - new_x2) + (new_y1 - new_y2) * (new_y1 - new_y2);
		float dvdz = (vx1 - vx2) * (new_x1 - new_x2) + (vy1 - vy2) * (new_y1 - new_y2);

		float new_vx1 = vx1 - dvdz / dzdz * (new_x1 - new_x2);
		float new_vy1 = vy1 - dvdz / dzdz * (new_y1 - new_y2);

		float new_vx2 = vx2 - dvdz / dzdz * (new_x2 - new_x1);
		float new_vy2 = vy2 - dvdz / dzdz * (new_y2 - new_y1);

		p1.set_position(new_x1, new_y1, margin);
		p2.set_position(new_x2, new_y2, margin);
		p1.set_velocity(coef * new_vx1, coef * new_vy1);
		p2.set_velocity(coef * new_vx2, coef * new_vy2);

		// std::cout << new_vx1 << " " << new_vy1 << std::endl;
		// std::cout << new_vx2 << " " << new_vy2 << std::endl;

	} 
}


void Simulation::attract(Particle &p1, Particle &p2) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;

	float dr = dx * dx + dy * dy;
	float sdr = sqrt(dr);
	float f = 50.0f / dr * (p1.sign * p2.sign);

	float r12x = dx / sdr;
	float r12y = dy / sdr;

	p1.new_ax += f * r12x;
	p1.new_ay += f * r12y;
}