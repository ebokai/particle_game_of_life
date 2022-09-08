#include <SDL.h>
#include <vector>
#include "particles.h"
using namespace std;

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