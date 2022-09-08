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
	unsigned int R, G, B;
};

// MAIN SIMULATION CLASS
class Framework{
public:
	
	Framework(int n_, unsigned int width_, unsigned int height_);
	~Framework();
	
	int get_cell_ID(float x, float y, int size);
	float dbound(float d, int lim);
	
	void make_hash_table();
	void update_and_bound();
	void interact();
	void friction();
	void initialize(int n_groups, int pp_group);
	void main_loop();

private:
	int n;
	float dt = 0.01;
	unsigned int width;
	unsigned int height;
	unsigned int cell_size = 100;

	vector<Particle> particles;
	vector<vector<int>> forces;
	map<int, vector<int>> hash_table;
	
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