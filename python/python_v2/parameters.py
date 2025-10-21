import numpy as np
from numpy.random import randint as rint
import functions


def initialize_parameters(target_n, n_groups):

	# global parameters 
	dt = 0.01
	xlim = 16
	ylim = 9

	scale = 0.01/dt

	n_particles_per_group = int(target_n/n_groups)
	n = n_groups * n_particles_per_group
	print(f'Target N: {target_n} | N: {n}')

	# dynamic parameters
	repel_distance = xlim/15
	force_distance = 4.5
	repel_strength = 4000/target_n
	force_strength = 2000/target_n
	friction_strength = 10

	# limitations
	vmax = 2 * xlim / 30 / dt 
	delta_vmax = vmax/2
	Fmax = delta_vmax/dt

	print(f'Force Distance: {force_distance:.1f}')
	print(f'Force Strength: {force_strength:.1f}')
	print(f'Friction Strength: {friction_strength:.1f}')

	# initialization
	x,y,vx,vy,f = functions.initialize(xlim,ylim,n)
	colors = functions.colors(n_groups, n_particles_per_group)
	sizes = 10 * np.random.randint(1,10,n)
	ncol = np.ones(n)

	repel_distances = functions.block_matrix(
		repel_distance, 0.01, n_groups, n_particles_per_group)
	force_distances = functions.block_matrix_ri(
		1 + repel_distance, force_distance, n_groups, n_particles_per_group)
	repel_forces = functions.block_matrix(
		repel_strength, 0.01, n_groups, n_particles_per_group)
	interaction_forces = functions.block_matrix(
		0, force_strength, n_groups, n_particles_per_group, False)

	data = [x, y, vx, vy, f, colors, ncol, sizes]
	params = [n, dt, xlim, ylim, vmax, Fmax, friction_strength]
	forces = [repel_distances, force_distances, repel_forces, interaction_forces]

	return data, params, forces