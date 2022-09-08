#include <SDL.h>
#include <vector>

using namespace std;


struct Particle{
	float x, y;
	float vx, vy;
	unsigned int p_ID;
};

class Framework{
public:
	Framework(int n_, unsigned int width_, unsigned int height_);
	~Framework();
	void initialize();
	void main_loop();
private:
	int n;
	unsigned int width;
	unsigned int height;
	vector<Particle> particles;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
};

float fast_rsqrt(float x);
float fast_sqrt(float x);