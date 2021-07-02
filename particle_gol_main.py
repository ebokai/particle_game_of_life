
import particle_gol_functions as pgol_func
import matplotlib.pyplot as plt 
import matplotlib.animation as anim 
import numpy as np
from numpy.random import randint as rint

def init():
	particles.set_offsets([])
	return particles,

def animate(i):

	global x, y, vx, vy, colors, ncol, sizes

	x = pgol_func.bound(x,xlim)
	y = pgol_func.bound(y,ylim)

	r, a = pgol_func.distance(x, y, n, xlim, ylim)

	# vx, vy = pgol_func.avoid(r,vx,vy,1,0.01)
	# vx, vy = pgol_func.avoid_group(r,vx,vy,1,0.02)
	vx, vy = pgol_func.vmax(vx,vy,vmax)

	sizes = pgol_func.get_sizes_v(vx,vy,sizes)
	# sizes = pgol_func.get_sizes(r, sizes)


	fx, fy = pgol_func.attract_gaussian(f, r, a, repel_forces, interaction_forces, repel_distances, force_distances)

	Fx = fx
	Fy = fy
	Fx, Fy = pgol_func.vmax_zero(Fx, Fy, Fmax)	

	x += vx * dt
	y += vy * dt 

	vx -= Fx * dt
	vy -= Fy * dt
	vx = pgol_func.speedup(vx)
	vy = pgol_func.speedup(vy)
	vx, vy = pgol_func.friction(vx, vy, friction_strength, dt)


	colors, ncol = pgol_func.get_colors(r, colors, ncol)
	# new_colors = pgol_func.get_colors_v(r,vx,vy,colors)

	xy = np.vstack((x,y)).T

	particles.set_offsets(xy)
	particles.set_sizes(sizes)
	particles.set_color(colors)

	return particles,

# global parameters 
dt = 0.01
xlim = 24
ylim = 13.5

target_n = 400
n_groups = 4
n_particles_per_group = int(target_n/n_groups)
n = n_groups * n_particles_per_group
print(f'Target N: {target_n} | N: {n}')

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


print(f'Force Distance: {force_distance}')
print(f'Force Strength: {force_strength}')
print(f'Friction Strength: {friction_strength}')

# initialization
x,y,vx,vy,f = pgol_func.initialize(xlim,ylim,n)
colors = pgol_func.colors(n_groups, n_particles_per_group)
sizes = 20 * np.ones(n)
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
particles = ax.scatter(x, y, s=10, marker='o', c=colors, animated=True)
animation = anim.FuncAnimation(fig, animate, 
	init_func=init, interval=0, blit=True, save_count=0, cache_frame_data=False)

plt.tight_layout(pad=0)
plt.show()
