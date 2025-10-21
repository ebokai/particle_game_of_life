import functions
import numpy as np


def sim_loop(data, params, forces):

	[x, y, vx, vy, f, colors, ncol, sizes] = data
	[n, dt, xlim, ylim, vmax, fmax, fs] = params
	[r_dist, f_dist, r_force, i_force] = forces

	vx_old = vx.copy()
	vy_old = vy.copy()

	x = functions.bound(x, xlim)
	y = functions.bound(y, ylim)

	r, a = functions.distance(x, y, n, xlim, ylim)
	vx, vy = functions.vmax(vx, vy, vmax)
	v = vx * vx + vy * vy 



	# sizes = functions.get_sizes(r,sizes)
	ksizes = functions.get_sizes_xy(x,y,xlim,ylim,sizes)

	fx, fy = functions.attract_gaussian(f, r, a, r_force, i_force, r_dist, f_dist)
	ffx, ffy = functions.friction(vx, vy, v, fs)

	Fx = ffx + fx
	Fy = ffy + fy

	Fx, Fy = functions.vmax(Fx, Fy, fmax)


	x += vx * dt
	y += vy * dt 

	vx -= Fx * dt
	vy -= Fy * dt

	dv = (vx-vx_old)**2 + (vy-vy_old)**2

	# colors, ncol = functions.get_colors(r, dv, colors, ncol)
	colors = functions.get_colors_xy(x,y,xlim,ylim,colors)



	xy = np.vstack((x,y)).T

	data = [x, y, vx, vy, f, colors, ncol, sizes]

	return xy, ksizes, colors, data, forces