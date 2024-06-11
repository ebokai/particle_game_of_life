struct Particle{
	
	float x;
	float y;
	float vx = 1;
	float vy = 1;
	float ax = 0;
	float ay = 0;

	unsigned int label;
	unsigned int cell_x;
	unsigned int cell_y;

	float radius = 3;

	int R;
	int G;
	int B;

	void set_position(float new_x, float new_y, int margin) {
		x = new_x;
		y = new_y;
		update_cell(margin);
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
		update_cell(margin);
	}

	void bound_x(int width, int margin) {
		if (x > width + margin) {
			x = x - (width + 2 * margin);
			update_cell(margin);
		}

		if (x < (0 - margin)) {
			x = x + (width + 2 * margin);
			update_cell(margin);
		}
	}

	void bound_y(int height, int margin) {
		if (y > height + margin) {
			y = y - (height + 2 * margin);
			update_cell(margin);
		}

		if (y < (0 - margin)) {
			y = y + (height + 2 * margin);
			update_cell(margin);
		}
	}

	void update_cell(int margin) {
		cell_x = x/margin;
		cell_y = y/margin;
	}

};
