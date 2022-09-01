#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <time.h>

using namespace std;


const int n_groups = 12;
const int pp_group = 150;
const int n = n_groups * pp_group;

const int DrawRadius = 2;
const double dt = 0.02;
const double dRepel = pow(30,2); // square of desired repulsion distance
const double dForce = pow(140,2);
const int FStrength = 10; // strength of force [-FStrength, FStrength]
const double FMult = 0.05; // additional multiplier (since low values of FStrength limit diversity)
const int RStrength = 200;
const double fs = 0.5;

// FUNCTIONS ====================================
vector<vector<double>> initialize_particles(vector<vector<double>> particles, int width, int height){

	int v0 = 1;
	srand(time(NULL));


	for (int i = 0; i < n; i++){
		particles[i][0] = static_cast <double> (rand()) / static_cast <double> (RAND_MAX/width); // x position
		particles[i][1] = static_cast <double> (rand()) / static_cast <double> (RAND_MAX/height); // y position
		particles[i][2] = -v0 + rand() / (RAND_MAX/v0/2); // x velocity
		particles[i][3] = -v0 + rand() / (RAND_MAX/v0/2); // y velocity
	}

	return particles;
}

vector<vector<int>> initialize_colors(vector<vector<int>> colors){
	for (int i = 0; i < n_groups; i++){
		int R = rand() / (RAND_MAX/255);
		int G = rand() / (RAND_MAX/255);
		int B = rand() / (RAND_MAX/255);
		for (int j = i * pp_group; j < (i+1) * pp_group; j++){
			colors[j][0] = R;
			colors[j][1] = G;
			colors[j][2] = B;

		}
	} 

	return colors;
}

vector<vector<double>> update_position(vector<vector<double>> particles){

	for (int i = 0; i < n; i++){
		particles[i][0] += particles[i][2] * dt;
		particles[i][1] += particles[i][3] * dt;
	}

	return particles;
}

vector<vector<double>> friction(vector<vector<double>> particles){

	double vx, vy, v, a;
	for (int i = 0; i < n; i++){
		vx = particles[i][2];
		vy = particles[i][3];
		v = sqrt(vx*vx + vy*vy);

		if (v > 2){
			a = atan2(vy, vx);
			particles[i][2] -= cos(a) * v * fs * dt;
			particles[i][3] -= sin(a) * v * fs * dt;
		}

	}

	return particles;
}

vector<vector<double>> bound(vector<vector<double>> particles, int width, int height){

	for (int i = 0; i < n; i++){
		double x = particles[i][0];
		double y = particles[i][1];

		if (x > width){
			x = x - width;
		}
		if (x < 0){
			x = x + width;
		}
		if (y > height){
			y = y - height;
		}
		if (y < 0){
			y = y + height;
		}

		particles[i][0] = x;
		particles[i][1] = y;
	}

	return particles;
}

vector<vector<int>> initialize_forces(vector<vector<int>> forces){

	for (int i = 0; i < n_groups; i++){
		for(int j = 0; j < n_groups; j++){
			int force = -FStrength/2 + rand() / (RAND_MAX/FStrength);
			for(int k = i * pp_group; k < (i+1) * pp_group; k++){
				for(int l = j * pp_group; l < (j+1) * pp_group; l++){
					forces[k][l] = force;
				}
			}
		}
	}

	return forces;

}

double dbound(double d, int lim){
	if (d > lim/2){
		d = d - lim;
	}
	if (d < -lim/2){
		d = d + lim;
	}
	return d;
}

vector<vector<double>> repel(vector<vector<double>> particles, int width, int height, vector<vector<int>> forces) {

	double x1, x2, y1, y2, dx, dy, dr, a, fx, fy, dR;
	double f = 0;

	for (int i = 0; i < n; i++){
		for (int j = i + 1; j < n; j++){

			f = 0;

			x1 = particles[i][0];
			y1 = particles[i][1];
			x2 = particles[j][0];
			y2 = particles[j][1];

			dx = x1 - x2;
			dy = y1 - y2;

			dx = dbound(dx, width);
			dy = dbound(dy, height);

			dR = dx * dx + dy * dy;

			if (dR < dRepel) {

				// only calculate if condition is met
				a = atan2(dy, dx);
				dr = sqrt(dR);
				f = RStrength * (sqrt(dRepel) - dr) / (dr + 1e-6f);
				fx = f * cos(a);
				fy = f * sin(a);

				particles[i][2] += fx * dt;
				particles[i][3] += fy * dt;
				particles[j][2] -= fx * dt;
				particles[j][3] -= fy * dt;
			}

			if ((dR > dRepel) && (dR < dForce)) {
				a = atan2(dy, dx);
				dr = sqrt(dR);
				f = FMult * forces[i][j] * (dr - sqrt(dRepel));
				fx = f * cos(a);
				fy = f * sin(a);
				particles[i][2] += fx * dt;
				particles[i][3] += fy * dt;
				particles[j][2] -= fx * dt;
				particles[j][3] -= fy * dt;
			}

		}
	}

	return particles;
}
// ==============================================

class Framework{
public:
	// initialize SDL
	Framework(int width_, int height_): height(height_), width(width_) {
		SDL_Init(SDL_INIT_VIDEO);
		window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	// destructor
	~Framework(){
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	// persistent window
	void main_loop(vector<vector<double>> particles, vector<vector<int>> colors, vector<vector<int>> forces){
		double x, y;
		int R, G, B;
		SDL_Event event;
		float secs;
		while (!(event.type == SDL_QUIT)){

			Uint64 start = SDL_GetPerformanceCounter();
			SDL_Delay(0);
			
			SDL_PollEvent(&event);
			if(event.type == SDL_QUIT) return;
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			particles = friction(particles);
			particles = bound(particles, width, height);
			particles = repel(particles, width, height, forces);
			particles = update_position(particles);


			for (int i = 0; i < n; i++){
				x = particles[i][0];
				y = particles[i][1];
				R = colors[i][0];
				G = colors[i][1];
				B = colors[i][2];
				draw_circle(x, y, DrawRadius, R, G, B);
				// draw_circle_Bres(x, y, DrawRadius, R, G, B);
			}

			SDL_RenderPresent(renderer);
			Uint64 end = SDL_GetPerformanceCounter();
			secs = (end - start) / (float) SDL_GetPerformanceFrequency();
			// cout << (1.0f) / secs << endl;
			//SDL_Delay(floor(16.666f - 1000.0f * secs)); // cap to 60 FPS
			
		}
	}

	// draw circular particle 
	void draw_circle(double X, double Y, int radius, int R, int G, int B){
		SDL_SetRenderDrawColor(renderer, R, G, B, 255);
		for (int x = X-radius; x <= X+radius; x++){
			for (int y = Y-radius; y <= Y+radius; y++){
				if((pow(X-x,2) + pow(Y-y,2)) <= pow(radius,2)){
					SDL_RenderDrawPoint(renderer, x, y);
				}
			}
		}

	}



private:
	int width;
	int height;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
};




int main(int argc, char *argv[]){

	int width = 1440;
	int height = 900;

	vector<vector<int>> forces(n, vector<int>(n));
	vector<vector<double>> particles(n, vector<double>(4));
	vector<vector<int>> colors(n,vector<int>(3));
	forces = initialize_forces(forces);
	particles = initialize_particles(particles, width, height);
	colors = initialize_colors(colors);

	Framework framework(width, height);
	framework.main_loop(particles, colors, forces);

	return 0;
}