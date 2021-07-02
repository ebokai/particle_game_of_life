import particle_gol_sim as pgol_sim
import particle_gol_params as pgol_params
import particle_gol_functions as pgol_func

import matplotlib.pyplot as plt 
import matplotlib.animation as anim 
import numpy as np
from numpy.random import randint as rint
import time

# initialize 
data, params, forces = pgol_params.params()
xlim, ylim = params[2:4]
x,y = data[:2]
colors = data[5]

# set up figure
plt.rcParams['axes.facecolor'] = '#000000'
xres = 1600; yres = 900
dpi = 72
width = xres/dpi
height = yres/dpi
fig, ax  = plt.subplots(figsize = (width,height), facecolor = 'k', dpi=dpi)
ax.set_aspect('equal')
ax.set_xlim(-xlim,xlim)
ax.set_ylim(-ylim,ylim)
particles = ax.scatter(x, y, s=10, c=colors, rasterized=True)

fps = 30
duration = 180 
frames = fps * duration
total_time = 0


for frame in range(frames):

	start = time.time()

	xy, sizes, colors, data = pgol_sim.sim_loop(data, params, forces)

	particles.set_offsets(xy)
	particles.set_sizes(sizes)
	particles.set_color(colors)

	plt.tight_layout(pad=0)
	fstr = f'{frame}'.zfill(5)
	plt.savefig(f'c:/users/hp/desktop/frames/frame_{fstr}.png',dpi=72,bbox_inches=None)

	end = time.time()

	elapsed = end - start 
	total_time += elapsed
	time_per_frame = total_time/(frame+1)
	remaining = (frames-frame)*time_per_frame
	remaining_minutes = remaining % 3600
	remaining_seconds = remaining_minutes % 60

	print(f'Saved frame: {frame+1}/{frames} | {elapsed:.1f} sec')
	print(f'{remaining/3600:.0f} hr {remaining_minutes/60:.0f} min {remaining_seconds:.0f} sec remaining')

