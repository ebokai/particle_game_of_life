import numpy as np
from numpy.random import randint as rint
import particle_gol_functions as pgol_func


def params():

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

	data = [x, y, vx, vy, f, colors, ncol, sizes]
	params = [n,dt,xlim,ylim,vmax,Fmax,friction_strength]
	forces = [repel_distances, force_distances, repel_forces, interaction_forces]

	return data, params, forces