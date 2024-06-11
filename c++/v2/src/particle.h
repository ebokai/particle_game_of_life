struct Particle{
	
	float x;
	float y;
	float vx = 0;
	float vy = 0;
	float ax = 0;
	float ay = 0;
	float new_ax = 0;
	float new_ay = 0;

	int group;

	unsigned int label;
	unsigned int cell_x;
	unsigned int cell_y;

	int x_cells;
	int y_cells;

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

	void update_velocity(float dt) {
		float new_vx = vx + 0.5f * (ax + new_ax) * dt;
		float new_vy = vy + 0.5f * (ay + new_ay) * dt;
		vx = new_vx;
		vy = new_vy;
		ax = new_ax;
		ay = new_ay;
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

		if ((cell_x < 0) || (cell_x > x_cells)) {
			cell_x = 0;
		}

		if ((cell_y < 0) || (cell_y > y_cells)) {
			cell_y = 0;
		}
	}

	void friction() {
		ax -= vx * 0.01;
		ay -= vy * 0.01;
	}

};
