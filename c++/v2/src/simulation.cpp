#include "simulation.h"

// constructor
Simulation::Simulation() 
	: forces(n_groups, std::vector<float>(n_groups)){	
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

	int tot_fps = 0;
	int frame = 0;

	while(!(event.type == SDL_QUIT)) {

		Uint64 start = SDL_GetPerformanceCounter();

		SDL_Delay(0);
		SDL_PollEvent(&event);

		if (event.type == SDL_KEYDOWN) {
			handle_key_press(event);
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		build_spatial_grid();
		interact(false);
		interact(true);

		for (auto &p : particles) {
			// p.friction();
			p.update_velocity(dt);
			// p.bound_x(width, margin);
			// p.bound_y(height, margin);

			if ((p.x > 0) & (p.x < width)) {
				if ((p.y > 0) & (p.y < height)) {
					draw_circle(renderer, &p);
				}
			}
		}

		SDL_RenderPresent(renderer);

		frame++;
		Uint64 end = SDL_GetPerformanceCounter();
		float secs = (end - start) / (float) SDL_GetPerformanceFrequency();
		float fps = (1.0f) / secs;
		tot_fps += fps;
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


void Simulation::initialize_sim(bool first_time) {

	// initialize RNG
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis_x(0.0f, static_cast<float>(width));
	std::uniform_real_distribution<float> dis_y(0.0f, static_cast<float>(height));
	std::uniform_real_distribution<float> dis_v(-5.0f, 5.0f);
	std::uniform_int_distribution<int> dis_c(0, 255);
	std::uniform_int_distribution<int> dis_s(0,2);
	std::uniform_real_distribution<float> dis_f(-1.0f, 1.0f);

	// initialize spatial grid
	spatial_grid.resize(x_cells);
	for (int i = 0; i < x_cells; i++) {
		spatial_grid[i].resize(y_cells);
		for (int j = 0; j < y_cells; j++) {
			spatial_grid[i][j].resize(0);
		}
	}

	// initialize particles and forces
	for (unsigned int i = 0; i < n_groups; i++) {
		
		// group color
		int R = dis_c(gen);
		int G = dis_c(gen);
		int B = dis_c(gen);

		// force matrix
		for (unsigned int j = 0; j < n_groups; j++) {
			forces[i][j] = dis_f(gen);
		}

		for (unsigned int j = 0; j < n_per_group; j++) {

			// actions skipped on reset (first_time = false)
			if (first_time) {
				float x = dis_x(gen);
				float y = dis_y(gen);
				float vx = 0; // dis_v(gen);
				float vy = 0; // dis_v(gen);

				Particle p;
				p.set_position(x, y, margin);
				p.set_velocity(vx, vy);
				p.set_color(R, G, B);
				
				p.x_cells = x_cells;
				p.y_cells = y_cells;
			
				p.label = i * n_per_group + j;
				p.group = i;
			
				particles.push_back(p);
			} else {
				Particle p = particles[i * n_per_group + j];
				p.set_color(R, G, B);
				particles[i * n_per_group + j] = p;
			}		
		}		
	}
}

void Simulation::draw_circle(SDL_Renderer *renderer, Particle *p) {

	SDL_SetRenderDrawColor(renderer, p->R, p->G, p->B, 255);

	int icx = static_cast<int>(p->x);
	int icy = static_cast<int>(p->y);
	int ird = static_cast<int>(particle_radius);

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
		p.new_ax = 0;
		p.new_ay = 0;
		p.update_pos(dt, width, height, margin);
		p.bound_x(width, margin);
		p.bound_y(height, margin);
		spatial_grid[p.cell_x][p.cell_y].push_back(p);	
	}
}


void Simulation::interact(bool collide_only) {

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
							if (!collide_only) {
								attract(p1, p2);
							} 
							
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


	float r = 2 * collide_radius;
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

		float new_x1 = mx - collide_radius * ux;
		float new_y1 = my - collide_radius * uy;
		float new_x2 = mx + collide_radius * ux;
		float new_y2 = my + collide_radius * uy;

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


float Simulation::potential(float r) {

	float rc = collide_radius;
	float k = atr_range/2;
	float fa = fabs(r - (rc + k))/k;
	fa = 1 - fa;

	return fa;
}

void Simulation::attract(Particle &p1, Particle &p2) {

	float dx = dbound(p2.x - p1.x, width);
	float dy = dbound(p2.y - p1.y, height);

	float dr = dx * dx + dy * dy;
	float sdr = sqrt(dr);
	float pot = 0;

	if (sdr > collide_radius) {
		if (sdr < collide_radius + atr_range) {
			pot = potential(sdr);
		}
	}

	float f = max_force * forces[p1.group][p2.group] * pot;

	float r12x = dx / sdr;
	float r12y = dy / sdr;

	p1.new_ax += f * r12x;
	p1.new_ay += f * r12y;
}

float Simulation::dbound(float dx, int lim) {
	if (dx > lim/2) {
		dx = dx - lim;
	}
	if (dx < -lim/2) {
		dx = dx + lim;
	}

	return dx;
}

void Simulation::handle_key_press(SDL_Event &event) {
	switch(event.key.keysym.sym) {
	case 114:
		initialize_sim(false);
		break;
	case 45:
		if (max_force >= 0.5) {
			max_force -= 0.5;
			std::cout << "max force: " << max_force << std::endl;
		}
		break;
	case 61:
		if (max_force <= 19.5) {
			max_force += 0.5;
			std::cout << "max force: " << max_force << std::endl;
		}
		break;
	case 91:
		if (atr_range > 0) {
			atr_range -= 1;
			std::cout << "attract range: " << atr_range << std::endl;
		}
		break;
	case 93: 
		if (atr_range < margin) {
			atr_range += 1;
			std::cout << "attract range: " << atr_range << std::endl;
		}
		break;
	case 39:
		if (coef >= 0.0025) {
			coef -= 0.0025;
			std::cout << "elasticity: " << coef << std::endl;
		}
		break;
	case 92:
		if (coef <= 0.9975) {
			coef += 0.0025;
			std::cout << "elasticity: " << coef << std::endl;
		}
		break;
	case 46:
		if (collide_radius >= particle_radius + 1) {
			collide_radius -= 1.0f;
			std::cout << "collide_radius: " << collide_radius << std::endl;
		}
		break;
	case 47:
		if (collide_radius <= 19) {
			collide_radius += 1.0f;
			std::cout << "collide_radius: " << collide_radius << std::endl;
		}
		break;
	case 102:
		std::cout << "average frame rate: " << tot_fps/frame << std::endl;
		break;

	default:
		std::cout << event.key.keysym.sym << std::endl;
	}
}