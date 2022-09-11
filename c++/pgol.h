#include <SDL.h>
#include <vector>
#include <cmath>
#include <map>
#include <iostream>

using namespace std;

const float pi = 3.14159265f;
const float pi_2 = 1.57079633f;

// PARTICLE STRUCTURE 
struct Particle{
	float x, y;
	float vx, vy;
	unsigned int p_ID;
	unsigned int R, G, B, A;
};

// MAIN SIMULATION CLASS
class Framework{
public:
	
	Framework(unsigned int width_, unsigned int height_);
	~Framework();
	
	unsigned int get_cell_ID(float x, float y);
	float dbound(float d, int lim);
	
	void make_hash_table();
	void update_and_bound();
	void interact();
	void friction();
	void initialize();
	void main_loop();

private:
	float dt = 0.01;
	unsigned int width;
	unsigned int height;

	// performance parameters
	unsigned int cell_size = 100;
	unsigned int n_target = 3500;
	unsigned int n_groups = 10;
	unsigned int pp_group = n_target / n_groups;
	unsigned int n = n_groups * pp_group;
	unsigned int max_interactions = 10;

	// simulation parameters
	float dRepel = 30;
	float dRepelSq = dRepel * dRepel;
	float repel_strength = 400;

	float dForce = 70;
	float dForceSq = dForce * dForce;

	float friction_strength = 15;

	// containers
	vector<Particle> particles;
	vector<vector<int>> forces;
	map<unsigned int, vector<unsigned int>> hash_table;

	// SDL objects
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
};

// HELPER FUNCTIONS
float fast_rsqrt(float x);
float fast_sqrt(float x);
float atan_approx(float x);
float fast_sin(float x);
float fast_cos(float x);
float fast_atan2(float y, float x);
float xbound(float x, unsigned int lim);