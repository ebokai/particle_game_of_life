
import particle_gol_functions as pgol_func
import matplotlib.pyplot as plt 
import matplotlib.animation as anim 
import numpy as np
from numpy.random import randint as rint
import time

# global parameters 
dt = 0.005
xlim = 32
ylim = 18
n_groups = 20
n_particles_per_group = 100
n = n_groups * n_particles_per_group

# dynamic parameters
repel_distance = 1
force_distance = rint(8,10)
repel_strength = 250
force_strength = rint(10,50)
friction_strength = rint(5,10)

# limitations
vmax = 100
delta_vmax = vmax/10
Fmax = delta_vmax/dt

# initialization
x,y,vx,vy,f = pgol_func.initialize(xlim,ylim,n)
colors = pgol_func.colors(n_groups, n_particles_per_group)
sizes = np.ones(n)
ncol = np.ones(n)

repel_distances = pgol_func.block_matrix(
	repel_distance, 0.01, n_groups, n_particles_per_group)
force_distances = pgol_func.block_matrix_ri(
	repel_distance, force_distance, n_groups, n_particles_per_group)
repel_forces = pgol_func.block_matrix(
	repel_strength, 0.01, n_groups, n_particles_per_group)
interaction_forces = pgol_func.block_matrix(
	0, force_strength, n_groups, n_particles_per_group, False)


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

	x = pgol_func.bound(x,xlim)
	y = pgol_func.bound(y,ylim)

	r, a = pgol_func.distance(x, y, n, xlim, ylim)
	
	vx, vy = pgol_func.vmax(vx,vy,vmax)
	sizes = pgol_func.get_sizes_v(vx,vy,sizes)

	Fx, Fy = pgol_func.attract_gaussian(f, r, a, repel_forces, interaction_forces, repel_distances, force_distances)
	Fx, Fy = pgol_func.vmax_zero(Fx,Fy,Fmax)

	x += vx * dt
	y += vy * dt 

	vx -= Fx * dt
	vy -= Fy * dt

	vx = pgol_func.speedup(vx)
	vy = pgol_func.speedup(vy)
	vx, vy = pgol_func.friction(vx, vy, friction_strength, dt)

	colors, ncol = pgol_func.get_colors(r,colors,ncol)

	xy = np.vstack((x,y)).T

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

