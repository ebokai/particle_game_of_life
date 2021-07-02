import particle_gol_functions as pgol_func
import numpy as np


def sim_loop(data,params,forces):

	[x, y, vx, vy, f, colors, ncol, sizes] = data
	[n, dt, xlim, ylim, vmax, fmax, friction_strength] = params
	[repel_distances, force_distances, repel_forces, interaction_forces] = forces

	x = pgol_func.bound(x,xlim)
	y = pgol_func.bound(y,ylim)

	r, a = pgol_func.distance(x, y, n, xlim, ylim)
	vx, vy = pgol_func.vmax(vx,vy,vmax)

	sizes = pgol_func.get_sizes_v(vx,vy,sizes)

	fx, fy = pgol_func.attract_gaussian(f, r, a, repel_forces, interaction_forces, repel_distances, force_distances)
	fx, fy = pgol_func.vmax_zero(fx, fy, fmax)	

	x += vx * dt
	y += vy * dt 

	vx -= fx * dt
	vy -= fy * dt
	vx = pgol_func.speedup(vx)
	vy = pgol_func.speedup(vy)
	vx, vy = pgol_func.friction(vx, vy, friction_strength, dt)

	colors, ncol = pgol_func.get_colors(r, colors, ncol)

	xy = np.vstack((x,y)).T
	
	data = [x, y, vx, vy, f, colors, ncol, sizes]

	return xy, sizes, colors, data
