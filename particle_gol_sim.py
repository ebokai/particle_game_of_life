import particle_gol_functions as pgol_func
import numpy as np


def sim_loop(data, params, forces):

	[x, y, vx, vy, f, colors, ncol, sizes] = data
	[n, dt, xlim, ylim, vmax, fmax, fs] = params
	[r_dist, f_dist, r_force, i_force] = forces

	vx_old = vx.copy()
	vy_old = vy.copy()

	x = pgol_func.bound(x, xlim)
	y = pgol_func.bound(y, ylim)

	r, a = pgol_func.distance(x, y, n, xlim, ylim)
	vx, vy = pgol_func.vmax(vx, vy, vmax)
	v = vx * vx + vy * vy 



	# sizes = pgol_func.get_sizes(r,sizes)
	ksizes = pgol_func.get_sizes_xy(x,y,xlim,ylim,sizes)

	fx, fy = pgol_func.attract_gaussian(f, r, a, r_force, i_force, r_dist, f_dist)
	ffx, ffy = pgol_func.friction(vx, vy, v, fs)

	Fx = ffx + fx
	Fy = ffy + fy

	Fx, Fy = pgol_func.vmax(Fx, Fy, fmax)


	x += vx * dt
	y += vy * dt 

	vx -= Fx * dt
	vy -= Fy * dt

	dv = (vx-vx_old)**2 + (vy-vy_old)**2

	# colors, ncol = pgol_func.get_colors(r, dv, colors, ncol)
	colors = pgol_func.get_colors_xy(x,y,xlim,ylim,colors)



	xy = np.vstack((x,y)).T

	data = [x, y, vx, vy, f, colors, ncol, sizes]

	return xy, ksizes, colors, data, forces