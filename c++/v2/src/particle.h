
struct Particle{
	
	float x;
	float y;
	float vx = 1;
	float vy = 1;
	float ax = 0;
	float ay = 0;

	unsigned int label;
	float radius = 25;

	int R;
	int G;
	int B;

	void set_position(float new_x, float new_y) {
		x = new_x;
		y = new_y;
	}

	void set_velocity(float new_vx, float new_vy) {
		vx = new_vx;
		vy = new_vy;
	}

	void set_color(int red, int green, int blue) {
		R = red;
		G = green;
		B = blue;
	}

	void update_pos(float dt) {
		float new_x = x + vx * dt + 0.5f * ax * dt * dt;
		float new_y = y + vy * dt + 0.5f * ay * dt * dt;
		x = new_x;
		y = new_y;
	}

};