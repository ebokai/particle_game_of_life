struct Particle{
	
	float x;
	float y;
	float vx = 1;
	float vy = 1;
	float ax = 0;
	float ay = 0;

	unsigned int label;
	
	float radius = 10;

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

	void update_pos(float dt, int width, int height, int margin) {
		float new_x = x + vx * dt + 0.5f * ax * dt * dt;
		float new_y = y + vy * dt + 0.5f * ay * dt * dt;
		x = new_x;
		y = new_y;
		bound_x(width, margin);
		bound_y(height, margin);
	}

	void bound_x(int width, int margin) {
		if (x > width + margin) {
			x = x - (width + 2 * margin);
		}

		if (x < (0 - margin)) {
			x = x + (width + 2 * margin);
		}
	}

	void bound_y(int height, int margin) {
		if (y > height + margin) {
			y = y - (height + 2 * margin);
		}

		if (y < (0 - margin)) {
			y = y + (height + 2 * margin);
		}
	}

};
