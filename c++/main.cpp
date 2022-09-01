#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <time.h>
#include <immintrin.h>

using namespace std;
const float pi = 3.14159265f;
const float pi_2 = 1.57079633f;

const unsigned int n_groups = 10;
const unsigned int pp_group = 300;
const unsigned int n = n_groups * pp_group;

const unsigned int DrawRadius = 2;
const float dt = 0.02f;
const float dRepel = pow(60,2); // square of desired repulsion distance
const float dForce = pow(250,2);
const int FStrength = 20; // strength of force [-FStrength, FStrength]
const float FMult = 0.025f; // additional multiplier (since low values of FStrength limit diversity)
const unsigned int RStrength = 400; // repulsion strength
const float fs = 10.0f; // friction strength 

// FUNCTIONS ====================================

float fast_rsqrt(float x){
	return _mm_cvtss_f32( _mm_rsqrt_ss( _mm_set_ss(x)));
}

float fast_sqrt(float x){
	return x * fast_rsqrt(x);
}

inline float atan_approx(float x){
	float a1 = 0.99997726f;
	float a3 = -0.33262347f;
	float a5 = 0.19354346f;
	float a7 = -0.11643287f;
	float a9 = 0.05265332f;
	float a11 = -0.01172120f;
	float xx = x*x;

	return x * (a1 + xx * (a3 + xx * (a5 + xx * (a7 + xx * (a9 + xx * a11)))));
}

float fast_sin(float x){
	float a3 = -0.16666667f;
	float a5 = 0.00833333f;
	float a7 = -0.00019841f;
	float xx = x*x;
	return x * (1.0f + xx * (a3 + xx * (a5 + xx * a7)));
}

float fast_cos(float x){
	float a2 = -0.5f;
	float a4 = 0.04166667f;
	float a6 = -0.00138889f;
	float a8 = 0.00002480f;
	float xx = x*x;
	return 1.0f + xx * (a2 + xx * (a4 + xx * (a6 + xx * a8)));
}

float fast_atan2(float y, float x){

	bool swap = fabs(x) < fabs(y);
	float atan_input = (swap ? x : y) / (swap ? y : x);
	float res = atan_approx(atan_input);
	res = swap ? (atan_input >= 0.0f ? pi_2 : -pi_2) - res : res;
	if (x < 0.0f) {
		res = (y >= 0.0f ? pi : -pi) + res;
	}


	return res;

}

vector<vector<float>> initialize_particles(vector<vector<float>> particles, unsigned int width, unsigned int height){

	int v0 = 100;

	for (unsigned int i = 0; i < n; i++){
		particles[i][0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/width); // x position
		particles[i][1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/height); // y position
		particles[i][2] = -v0 + rand() / (RAND_MAX/v0/2); // x velocity
		particles[i][3] = -v0 + rand() / (RAND_MAX/v0/2); // y velocity
	}

	return particles;
}

vector<vector<unsigned int>> initialize_colors(vector<vector<unsigned int>> colors){
	for (unsigned int i = 0; i < n_groups; i++){
		unsigned int R = rand() / (RAND_MAX/255);
		unsigned int G = rand() / (RAND_MAX/255);
		unsigned int B = rand() / (RAND_MAX/255);
		for (unsigned int j = i * pp_group; j < (i+1) * pp_group; j++){
			colors[j][0] = R;
			colors[j][1] = G;
			colors[j][2] = B;

		}
	} 

	return colors;
}

vector<vector<float>> update_position(vector<vector<float>> particles){

	for (unsigned int i = 0; i < n; i++){
		particles[i][0] += particles[i][2] * dt;
		particles[i][1] += particles[i][3] * dt;
	}

	return particles;
}

vector<vector<float>> friction(vector<vector<float>> particles){

	float vx, vy, v, a;
	for (unsigned int i = 0; i < n; i++){
		vx = particles[i][2];
		vy = particles[i][3];
		v = fast_sqrt(vx*vx + vy*vy);		
		a = fast_atan2(vy, vx);
		particles[i][2] -= fast_cos(a) * v * fs * dt;
		particles[i][3] -= fast_sin(a) * v * fs * dt;
		

	}

	return particles;
}



vector<vector<float>> bound(vector<vector<float>> particles, unsigned int width, unsigned int height){

	for (unsigned int i = 0; i < n; i++){
		float x = particles[i][0];
		float y = particles[i][1];

		if (x > width){
			x = x - width;
			particles[i][0] = x;
		}
		if (x < 0){
			x = x + width;
			particles[i][0] = x;
		}
		if (y > height){
			y = y - height;
			particles[i][1] = y;
		}
		if (y < 0){
			y = y + height;
			particles[i][1] = y;
		}
		
	}

	return particles;
}

vector<vector<int>> initialize_forces(vector<vector<int>> forces){

	for (unsigned int i = 0; i < n_groups; i++){
		for(unsigned int j = 0; j < n_groups; j++){
			int force = -FStrength/2 + rand() / (RAND_MAX/FStrength);
			for(unsigned int k = i * pp_group; k < (i+1) * pp_group; k++){
				for(unsigned int l = j * pp_group; l < (j+1) * pp_group; l++){
					forces[k][l] = force;
				}
			}
		}
	}

	return forces;

}

float dbound(float d, int lim){
	if (d > lim/2){
		d = d - lim;
	}
	if (d < -lim/2){
		d = d + lim;
	}
	return d;
}

vector<vector<float>> repel(vector<vector<float>> particles, unsigned int width, unsigned int height, vector<vector<int>> forces) {

	float x1, x2, y1, y2, dx, dy, dr, a, fx, fy, dR;
	float f = 0.0f;

	for (unsigned int i = 0; i < n; i++){
		
		x1 = particles[i][0];
		y1 = particles[i][1];
		
		for (unsigned int j = i + 1; j < n; j++){

			f = 0.0f;

			
			x2 = particles[j][0];
			y2 = particles[j][1];

			dx = x1 - x2;
			dy = y1 - y2;

			dx = dbound(dx, width);
			dy = dbound(dy, height);

			dR = dx * dx + dy * dy;

			if (dR < dRepel) {

				// only calculate if condition is met
				a = fast_atan2(dy, dx);
				dr = fast_sqrt(dR);
				f = RStrength * (fast_sqrt(dRepel) - dr) / (dr + 1e-6f);
				fx = f * fast_cos(a);
				fy = f * fast_sin(a);

				particles[i][2] += fx * dt;
				particles[i][3] += fy * dt;
				particles[j][2] -= fx * dt;
				particles[j][3] -= fy * dt;
			}

			else if ((dR > dRepel) && (dR < dForce)) {
				a = fast_atan2(dy, dx);
				dr = fast_sqrt(dR);
				f = FMult * forces[i][j] * (dr - fast_sqrt(dRepel));
				fx = f * fast_cos(a);
				fy = f * fast_sin(a);
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
	Framework(unsigned int width_, unsigned int height_): width(width_), height(height_) {
		SDL_Init(SDL_INIT_VIDEO);
		window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		
		SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
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
	void main_loop(vector<vector<float>> particles, vector<vector<unsigned int>> colors, vector<vector<int>> forces){
		float x, y;
		unsigned int R, G, B;
		SDL_Event event;
		float secs;
		float fps;
		float tot_fps = 0;

		unsigned frame = 0;

		while (!(event.type == SDL_QUIT)){


			Uint64 start = SDL_GetPerformanceCounter();
			SDL_Delay(0);
			SDL_PollEvent(&event);
			if(event.type == SDL_QUIT) return;
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			// simulation functions
			particles = friction(particles);
			particles = bound(particles, width, height);
			particles = repel(particles, width, height, forces);
			particles = update_position(particles);

			// draw particles
			// for (unsigned int i = 0; i < n; i++){
			// 	x = particles[i][0];
			// 	y = particles[i][1];
			// 	R = colors[i][0];
			// 	G = colors[i][1];
			// 	B = colors[i][2];
			// 	draw_circle(x, y, DrawRadius, R, G, B);
			// 	// draw_circle_exp(x, y, R, G, B);
			// }

			for(unsigned int i = 0; i < n_groups; i++){
				R = colors[i*pp_group][0];
				G = colors[i*pp_group][1];
				B = colors[i*pp_group][2];
				SDL_SetRenderDrawColor(renderer, R, G, B, 255);
				for(unsigned int j = 0; j < pp_group; j++){
					unsigned int k = i*pp_group + j;
					x = particles[k][0];
					y = particles[k][1];
					draw_circle(x, y, DrawRadius);

				}
			}


			SDL_RenderPresent(renderer);

			Uint64 end = SDL_GetPerformanceCounter();
			secs = (end - start) / (float) SDL_GetPerformanceFrequency();
			frame++;
			fps = (1.0f) / secs;
			tot_fps += fps;
			cout << tot_fps/frame << endl;
			//SDL_Delay(floor(16.666f - 1000.0f * secs)); // cap to 60 FPS
			
		}
	}

	// draw circular particle 
	void draw_circle(float X, float Y, int radius){
		// SDL_SetRenderDrawColor(renderer, R, G, B, 255);
		for (int x = X-radius; x <= X+radius; x++){
			for (int y = Y-radius; y <= Y+radius; y++){

				float X2 = (X*X) - 2 * (x*X) + (x*x);
				float Y2 = (Y*Y) - 2 * (y*Y) + (y*y);

				if((X2 + Y2) <= (radius*radius)){
					SDL_RenderDrawPoint(renderer, x, y);
				}
			}
		}

	}

	void draw_circle_exp(float X, float Y, unsigned int R, unsigned int G, unsigned int B){
		SDL_SetRenderDrawColor(renderer, R, G, B, 255);
		int x = X;
		int y = Y;
		SDL_RenderDrawPoint(renderer, x, y);
		SDL_RenderDrawPoint(renderer, x+1, y);
		SDL_RenderDrawPoint(renderer, x-1, y);
		SDL_RenderDrawPoint(renderer, x, y+1);
		SDL_RenderDrawPoint(renderer, x, y-1);
		SDL_RenderDrawPoint(renderer, x+2, y);
		SDL_RenderDrawPoint(renderer, x-2, y);
		SDL_RenderDrawPoint(renderer, x, y+2);
		SDL_RenderDrawPoint(renderer, x, y-2);
		SDL_RenderDrawPoint(renderer, x+1, y+1);
		SDL_RenderDrawPoint(renderer, x+1, y-1);
		SDL_RenderDrawPoint(renderer, x-1, y+1);
		SDL_RenderDrawPoint(renderer, x-1, y-1);



	}



private:
	unsigned int width;
	unsigned int height;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
};




int main(int argc, char *argv[]){

	srand(time(NULL));

	unsigned int width = 1440;
	unsigned int height = 900;

	vector<vector<int>> forces(n, vector<int>(n));
	vector<vector<float>> particles(n, vector<float>(4));
	vector<vector<unsigned int>> colors(n,vector<unsigned int>(3));
	forces = initialize_forces(forces);
	particles = initialize_particles(particles, width, height);
	colors = initialize_colors(colors);

	Framework framework(width, height);
	framework.main_loop(particles, colors, forces);

	return 0;
}