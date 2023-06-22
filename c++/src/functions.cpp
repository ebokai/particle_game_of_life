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

float xbound(float x, unsigned int lim){
	if (x > lim){
		x = x - lim;
	}
	if (x < 0){
		x = x + lim;
	}

	return x;
}

// FRAMEWORK METHODS =====
unsigned int Framework::get_cell_ID(float x, float y){
	unsigned int xs = x/cell_size;
	unsigned int ys = y/cell_size;
	unsigned int ws = width/cell_size;
	return xs + ys * ws;
}

void Framework::make_hash_table(){
	map<unsigned int, vector<unsigned int>> new_table;
	float x, y;
	for (unsigned int i = 0; i < n; i++){
		Particle p = particles[i];
		x = p.x;
		y = p.y;
		unsigned int cell_ID = get_cell_ID(x, y);
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
	for (unsigned int i = 0; i < n; i++){
		float v = fast_sqrt(particles[i].vx * particles[i].vx + particles[i].vy * particles[i].vy);
		float a = fast_atan2(particles[i].vy, particles[i].vx);
		particles[i].vx -= fast_cos(a) * v * friction_strength * dt;
		particles[i].vy -= fast_sin(a) * v * friction_strength * dt;
	}
}


void Framework::interact(){

	unsigned int n_cells = get_cell_ID(width, height-1);
	unsigned int n_interactions;

	float dx, dy, dr, dR;
	float f, fx, fy, a;

	unsigned int neighbours[9];

	for (unsigned int i = 0; i < n_cells; i++){

		// CHECK CENTER CELL FOR PARTICLES

		vector<unsigned int> this_cell = hash_table[i];

		unsigned int n_this = this_cell.size();

		if (n_this == 0) continue;

		// FIND NEIGHBOURS USING REFERENCE PARTICLE
		unsigned int p_id = this_cell[0];
		Particle p_ref = particles[p_id];

		// THIS COULD BE DONE ONCE AT INITIALIZATION
		// ALSO, THE CELL INDEX COULD BE STORED IN THE PARTICLE STRUCT
		

		neighbours[0] = i;
		neighbours[1] = get_cell_ID(xbound(p_ref.x-cell_size, width), xbound(p_ref.y+cell_size, height));
		neighbours[2] = get_cell_ID(xbound(p_ref.x, width), xbound(p_ref.y+cell_size, height));
		neighbours[3] = get_cell_ID(xbound(p_ref.x+cell_size, width), xbound(p_ref.y+cell_size, height));
		neighbours[4] = get_cell_ID(xbound(p_ref.x-cell_size, width), xbound(p_ref.y, height));
		neighbours[5] = get_cell_ID(xbound(p_ref.x+cell_size, width), xbound(p_ref.y, height));
		neighbours[6] = get_cell_ID(xbound(p_ref.x-cell_size, width), xbound(p_ref.y-cell_size, height));
		neighbours[7] = get_cell_ID(xbound(p_ref.x, width), xbound(p_ref.y-cell_size, height));
		neighbours[8] = get_cell_ID(xbound(p_ref.x+cell_size, width), xbound(p_ref.y-cell_size, height));

		for (unsigned int j = 0; j < 9; j++){

			// LOOP OVER ADJACENT CELLS
			unsigned int that_cell_id = neighbours[j];
			vector<unsigned int> that_cell = hash_table[that_cell_id];
			unsigned int n_that = that_cell.size();

			if (n_that == 0) continue;

			for(unsigned int i1 = 0; i1 < n_this; i1++){

				unsigned int p1_ID = this_cell[i1];
				Particle p1 = particles[p1_ID];
				
				n_interactions = 0;

				for(unsigned int i2 = 0; i2 < n_that; i2++){

					if (n_interactions > max_interactions) continue;
					
					unsigned int p2_ID = that_cell[i2];

					if (p1_ID == p2_ID) continue;

					Particle p2 = particles[p2_ID];

					

					f = 0;

					dx = dbound(p1.x - p2.x, width);
					dy = dbound(p1.y - p2.y, height); 

					dR = dx*dx + dy*dy;

					if ((dR > 0) && (dR < dRepelSq)){
						//a = fast_atan2(dy, dx);
						dr = fast_sqrt(dR);
						f = repel_strength * (dRepel - dr) / (dr + 1);
						//fx = f * fast_cos(a);
						//fy = f * fast_sin(a);
						// this can also be implemented using the [dx,dy] unit vector
						// avoids cos, sin and atan
						fx = f * dx/dr;
						fy = f * dy/dr;

						p1.vx += fx * dt;
						p1.vy += fy * dt;
						n_interactions++;
					}
					if ((dR > dRepelSq) && (dR < dForceSq)) {
						//a = fast_atan2(dy, dx);
						dr = fast_sqrt(dR);
						f = forces[p1_ID][p2_ID] * (dr - dRepel);
						//fx = f * fast_cos(a);
						//fy = f * fast_sin(a);
						fx = f * dx/dr;
						fy = f * dy/dr;

						p1.vx += fx * dt;
						p1.vy += fy * dt;
						n_interactions++;							
					}
				}
				particles[p1_ID] = p1;
			}	
		}	
	}
}



