#include <immintrin.h>
#include "pgol.h"

float fast_rsqrt(float x){
	// fast reciprocal square root
	return _mm_cvtss_f32( _mm_rsqrt_ss( _mm_set_ss(x)));
}

float fast_sqrt(float x){
	// fast square root
	return x * fast_rsqrt(x);
}

float atan_approx(float x){
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

float xbound(float x, int lim){
	if (x > lim){
		x = x - lim;
	}
	if (x < 0){
		x = x + lim;
	}

	return x;
}

// FRAMEWORK METHODS =====
int Framework::get_cell_ID(float x, float y){
	int xs = int(x/cell_size);
	int ys = int(y/cell_size);
	int ws = int(width/cell_size);
	return xs + ys * ws;
}

void Framework::make_hash_table(){
	map<int, vector<int>> new_table;
	float x, y;
	for (int i = 0; i < n; i++){
		Particle p = particles[i];
		x = p.x;
		y = p.y;
		int cell_ID = get_cell_ID(x, y);
		new_table[cell_ID].push_back(p.p_ID);
	}
	hash_table = new_table; // update hash_table
}

void Framework::update_and_bound(){
	for (unsigned int i = 0; i < n; i++){
		Particle p = particles[i];
		p.x += p.vx * dt;
		p.y += p.vy * dt;
		if (p.x > width){
			p.x = p.x - width;
		}
		if (p.x < 0){
			p.x = p.x + width;
		}
		if (p.y > height){
			p.y = p.y - height;
		}
		if (p.y < 0){
			p.y = p.y + height;
		}
		particles[i] = p;
	}
}

void Framework::interact(){

	// TODO: LOOP OVER NEIGHBOURING CELLS

	map<int, vector<int>>::iterator it;

	float dx, dy, dr, dR, f, fx, fy, a;
	Particle p1, p2;

	for (it = hash_table.begin(); it != hash_table.end(); it++){

		int this_cell = it->first;

		vector<int> cell_particles = it->second;
		
		int n_particles = cell_particles.size();

		if (n_particles > 0){
		
			for (int i = 0; i < n_particles; i++){

				int ii = cell_particles[i];

				p1 = particles[ii];
				
				for (int j = i + 1; j < n_particles; j++){

					f = 0;

					int jj = cell_particles[j];
				
					p2 = particles[jj];

					dx = p1.x - p2.x;
					dy = p1.y - p2.y;

					dx = dbound(dx, width);
					dy = dbound(dy, height);

					dR = dx * dx + dy * dy; 

					if (dR < 400){
						a = fast_atan2(dy, dx);
						dr = fast_sqrt(dR);
						f = 50 * (20 - dr) / (dr + 1);

						fx = f * fast_cos(a);
						fy = f * fast_sin(a);

						p1.vx += fx * dt;
						p1.vy += fy * dt;
						p2.vx -= fx * dt;
						p2.vy -= fy * dt;
					}

					if ((dR > 400) && (dR < 900)) {

						a = fast_atan2(dy, dx);
						dr = fast_sqrt(dR);

						f = forces[ii][jj] * (dr - 20);
						fx = f * fast_cos(a);
						fy = f * fast_sin(a);
						p1.vx += fx * dt;
						p1.vy += fy * dt;

						f = forces[jj][ii] * (dr - 20);
						fx = f * fast_cos(a);
						fy = f * fast_sin(a);
						p2.vx -= fx * dt;
						p2.vy -= fy * dt;
					}
					particles[cell_particles[j]] = p2;
				}
				particles[cell_particles[i]] = p1;
			}
		}
	}
}

float Framework::dbound(float d, int lim){
	if (d > lim/2){
		d = d - lim;
	}
	if (d < -lim/2){
		d = d + lim;
	}
	return d;
}

void Framework::friction(){
	float fs = 5;
	for (int i = 0; i < n; i++){
		float v = fast_sqrt(particles[i].vx * particles[i].vx + particles[i].vy * particles[i].vy);
		float a = fast_atan2(particles[i].vy, particles[i].vx);
		particles[i].vx -= fast_cos(a) * v * fs * dt;
		particles[i].vy -= fast_sin(a) * v * fs * dt;
	}
}


void Framework::interact_test(){



	int n_cells = get_cell_ID(width, height-1);

	float dx, dy, dr, dR;
	float f, fx, fy, a;

	for (int i = 0; i < n_cells; i++){

		// CHECK CENTER CELL FOR PARTICLES

		vector<int> this_cell = hash_table[i];

		int n_this = this_cell.size();



		if (n_this > 0){

			// FIND NEIGHBOURS USING REFERENCE PARTICLE
			int p_id = this_cell[0];
			Particle p_ref = particles[p_id];

			int neighbours[9];

			neighbours[0] = i;
			neighbours[1] = get_cell_ID(xbound(p_ref.x-cell_size, width), xbound(p_ref.y+cell_size, height));
			neighbours[2] = get_cell_ID(xbound(p_ref.x, width), xbound(p_ref.y+cell_size, height));
			neighbours[3] = get_cell_ID(xbound(p_ref.x+cell_size, width), xbound(p_ref.y+cell_size, height));
			neighbours[4] = get_cell_ID(xbound(p_ref.x-cell_size, width), xbound(p_ref.y, height));
			neighbours[5] = get_cell_ID(xbound(p_ref.x+cell_size, width), xbound(p_ref.y, height));
			neighbours[6] = get_cell_ID(xbound(p_ref.x-cell_size, width), xbound(p_ref.y-cell_size, height));
			neighbours[7] = get_cell_ID(xbound(p_ref.x, width), xbound(p_ref.y-cell_size, height));
			neighbours[8] = get_cell_ID(xbound(p_ref.x+cell_size, width), xbound(p_ref.y-cell_size, height));

			for (int j = 0; j < 9; j++){

				// LOOP OVER ADJACENT CELLS
				int that_cell_id = neighbours[j];
				vector<int> that_cell = hash_table[that_cell_id];
				int n_that = that_cell.size();



				if (n_that > 0){
					for(int i1 = 0; i1 < n_this; i1++){

						int p1_ID = this_cell[i1];
						Particle p1 = particles[p1_ID];
						
						for(int i2 = 0; i2 < n_that; i2++){
							
							int p2_ID = that_cell[i2];

							if (p1_ID != p2_ID){


							
								Particle p2 = particles[p2_ID];

	

								f = 0;

								dx = dbound(p1.x - p2.x, width);
								dy = dbound(p1.y - p2.y, height); 

								dR = dx*dx + dy*dy;

								if ((dR > 0) && (dR < 900)){
									a = fast_atan2(dy, dx);
									dr = fast_sqrt(dR);
									f = 100 * (30 - dr) / (dr + 1);
									fx = f * fast_cos(a);
									fy = f * fast_sin(a);
									p1.vx += fx * dt;
									p1.vy += fy * dt;



								}

								if ((dR > 900) && (dR < 2500)) {
									a = fast_atan2(dy, dx);
									dr = fast_sqrt(dR);
									f = forces[p1_ID][p2_ID] * (dr - 30);
									fx = f * fast_cos(a);
									fy = f * fast_sin(a);
									p1.vx += fx * dt;
									p1.vy += fy * dt;		

						
								}
							}


						}

						particles[p1_ID] = p1;
					}
				}


			}




		}
	}
}



