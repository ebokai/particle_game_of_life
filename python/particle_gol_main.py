import particle_gol_sim as pgol_sim
import particle_gol_params as pgol_params
import particle_gol_functions as pgol_func

import matplotlib.pyplot as plt 
import matplotlib.animation as anim 
import numpy as np
from numpy.random import randint as rint

def init():
	# quick fix because set_offsets requires a Nx2 array
	x = np.zeros((1,2))
	particles.set_offsets(x)
	return particles,

def animate(i):

	global data, params, forces

	xy, sizes, colors, data, forces = pgol_sim.sim_loop(data, params, forces)
	particles.set_offsets(xy)
	particles.set_sizes(sizes)
	particles.set_color(colors)

	return particles,

data, params, forces = pgol_params.params(target_n = 400, n_groups = 7)
xlim, ylim = params[2:4]
x,y = data[:2]
colors = data[5]

plt.rcParams['axes.facecolor'] = '#000000'
xres, yres = 1600, 900
dpi = 72
width = xres/dpi
height = yres/dpi
fig, ax = plt.subplots(figsize = (width, height), facecolor = 'k', dpi=dpi)
ax.set_aspect('equal')
ax.set_xlim(-xlim,xlim)
ax.set_ylim(-ylim,ylim)
particles = ax.scatter(x, y, s = 10, marker = 'o', c = colors, animated = True)
animation = anim.FuncAnimation(fig, animate, 
	init_func = init, interval = 0, blit = True, save_count = 0, cache_frame_data = False)

plt.tight_layout(pad = 0)
plt.show()
