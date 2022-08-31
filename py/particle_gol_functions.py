import numpy as np 
from numpy.random import uniform
import matplotlib.pyplot as plt 
import colorsys

def bound(x, lim):

	x[x >= lim] -= 2 * lim
	x[x < -lim] += 2 * lim 

	return x

def distance(x, y, n, xlim, ylim):

	dx = x.reshape(n,1) - x
	dy = y.reshape(n,1) - y

	dx = bound(dx,xlim)
	dy = bound(dy,ylim)

	r = dx*dx + dy*dy

	a = np.arctan2(dy,dx)

	return np.sqrt(r), a

def attract(f, r, a, repulsion_forces, forces, repel_distances, force_distances):

	force_ranges = repel_distances + force_distances

	i0 = np.where(r < repel_distances)
	i1 = np.where( (r > repel_distances) & (r < (force_ranges/2) ) )
	i2 = np.where( (r > force_ranges/2) & (r < force_ranges))

	f[i0] = ((repulsion_forces * (repel_distances - r))/(r+1e-6))[i0]
	f[i1] = (forces * (r - repel_distances))[i1]
	f[i2] = (-forces * (r - force_ranges))[i2]

	np.fill_diagonal(f,0)

	fx = np.sum(f * np.cos(a), axis = 0) 
	fy = np.sum(f * np.sin(a), axis = 0) 

	return fx, fy

def attract_gaussian(f, r, a, repulsion_forces, forces, repel_distances, force_distances):

	# gaussian repulsion force

	f = repulsion_forces/(1+np.exp(5*(r-repel_distances))) + forces * np.exp(-(r-force_distances)**2/np.sqrt(force_distances)/2)

	np.fill_diagonal(f,0)

	fx = np.sum(f * np.cos(a), axis = 0) 
	fy = np.sum(f * np.sin(a), axis = 0) 

	return fx, fy

def initialize(xlim, ylim, n):

	# initialize positions and velocities of particles

	sn = int(np.sqrt(n)+1)
	r = 0.95
	x = np.linspace(-r*ylim,r*ylim,sn)
	y = np.linspace(-r*ylim,r*ylim,sn)
	xx,yy = np.meshgrid(x,y)

	xx = xx.flatten()
	x = xx[:n]
	yy = yy.flatten()
	y = yy[:n]

	z = np.arange(np.alen(x))
	z = np.random.permutation(z)
	x = x[z]
	y = y[z]

	vx = uniform(-10,10,n)
	vy = uniform(-10,10,n)
	f = np.zeros((n,n))

	return x,y,vx,vy,f

def block_matrix(mean, width, n_groups, n_particles_per_group, positive = True):

	m = np.random.normal(mean, width, (n_groups, n_groups))
	m = m.repeat(n_particles_per_group, axis = 1).repeat(n_particles_per_group, axis = 0)

	if positive:
		return np.abs(m)
	else:
		return m

def block_matrix_ri(rmin, rmax, n_groups, n_particles_per_group, positive = True):

	m = np.random.uniform(rmin, rmax, (n_groups, n_groups))
	m = m.repeat(n_particles_per_group, axis = 1).repeat(n_particles_per_group, axis = 0)

	return m

def colors(n_groups, n_particles_per_group):
	cs = []
	for i in range(n_groups):
		color = colorsys.hsv_to_rgb(uniform(),1,1)
		for j in range(n_particles_per_group):
			cs.append(color)
	return cs

def avoid(r, vx, vy, r_threshold, multiplier):
	i = r < r_threshold
	j = np.sum(i,axis=1)
	k = np.where(j > 1)
	l = np.where(j < 2)
	vx[k] *= (1 - multiplier)
	vy[k] *= (1 - multiplier)
	vx[l] *= (1 + multiplier)
	vy[l] *= (1 + multiplier)
	return vx, vy

def avoid_group(r, vx, vy, r_threshold, multiplier):
	i = r < r_threshold
	j = np.sum(i,axis=1)
	n = j/np.amax(j)
	k = np.where(j > 1)
	l = np.where(j < 2)
	vx[k] *= (1 - n[k] * multiplier)
	vy[k] *= (1 - n[k] * multiplier)
	vx[l] *= (1 + n[l] * multiplier)
	vy[l] *= (1 + n[l] * multiplier)
	return vx, vy

def get_sizes(r, old_sizes):
	x = 0.25
	i = r < 2
	j = np.sum(i, axis = 1)
	# j = j/np.amax(j)

	sizes = 4 + 1/(j+1) * 1000
	sizes = (1-x) * old_sizes + x * sizes
	return sizes

def get_sizes_v(vx,vy,old_sizes):
	r = 0.25
	v = vx*vx + vy*vy
	av = np.mean(v)
	sv = np.std(v)
	s = np.abs(v-av)/sv
	sizes = 10 + s * 30
	sizes = (1-r) * old_sizes + r * sizes
	return sizes

def get_sizes_xy(x,y,xlim,ylim,sizes):
	# dx = np.abs(x+xlim)/xlim/2
	dy = np.abs(y+ylim)/ylim/2

	sizes = sizes + 10 * sizes * (1-dy)**6

	# sizes = np.full(10,np.alen(x))
	return sizes

def get_colors_xy(x,y,xlim,ylim,colors):
	dx = (xlim - np.abs(x))/xlim 
	dy = np.abs(y+ylim)/ylim/2
	cs = []
	for kx,ky,h in zip(dx,dy,colors):

		kr = (kx + ky)/2

		u = 1 - np.cos(2*(ky-3/4)*np.pi)
		v = 1 - 2 * np.abs(ky-1/2)
		h_hsv = colorsys.rgb_to_hsv(h[0],h[1],h[2])
		color = colorsys.hsv_to_rgb(h_hsv[0],0.25 + 0.75 * u/2,0.25 + 0.75 * v)
		cs.append((color[0],color[1],color[2],0.2+0.8*kr**2))
	return cs


def get_colors(r, v, colors, n_old):
	
	i = r < 2
	j = np.sum(i, axis = 1)
	
	n = j/np.amax(j)

	r = 0.25
	q = v/np.amax(v)

	n = (1-r) * n_old + r * n

	
	cs = []

	for m1,m2,h in zip(n,q,colors):
		h_hsv = colorsys.rgb_to_hsv(h[0],h[1],h[2])
		color = colorsys.hsv_to_rgb(h_hsv[0], 0.3 + 0.5 * m1 + 0.2 * m2, 0.3 + 0.5 * m1 + 0.2 * m2)
		cs.append((color[0],color[1],color[2],0.25 + 0.5 * m1 + 0.25 * m2))
	return cs, n

def get_colors_vhue(r, v, colors, n_old):
	
	i = r < 2
	j = np.sum(i, axis = 1)
	
	n = j/np.amax(j)

	r = 0.25
	q = v/np.amax(v)

	n = (1-r) * n_old + r * n

	
	cs = []

	for m1,m2,h in zip(n,q,colors):
		h_hsv = colorsys.rgb_to_hsv(h[0],h[1],h[2])
		color = colorsys.hsv_to_rgb(m2, 0.7 + 0.15 * m1 + 0.15 * m2, 0.7 + 0.15 * m1 + 0.15 * m2)
		cs.append((color[0],color[1],color[2],0.5 + 0.25 * m1 + 0.25 * m2))
	return cs, n

def get_colors_v(r,vx,vy,colors):

	v = vx*vx + vy*vy
	vxm = vx/np.sqrt(v)
	vym = vy/np.sqrt(v)
	
	i = r < 3
	j = np.sum(i, axis = 1)
	n = j / np.amax(j)
	cs = []

	mix = 0.3

	for m,xv,yv,h in zip(n,vxm,vym,colors):
		r = (1-mix)*h[0]+mix*xv
		b = (1-mix)*h[2]+mix*yv
		h_hsv = colorsys.rgb_to_hsv(r,h[1],b)
		color = colorsys.hsv_to_rgb(h_hsv[0],m,0.1 + 0.9*m)
		cs.append((color[0],color[1],color[2],0.2 + 0.8 * m))

	return cs

def friction(vx, vy, v, fs):

	a = np.arctan2(vy, vx)
	Fx = np.cos(a) * v * fs
	Fy = np.sin(a) * v * fs

	return Fx, Fy

def vmax(vx, vy, v_max):

	fast_vx = np.where(np.abs(vx) > v_max)
	fast_vy = np.where(np.abs(vy) > v_max)

	fast_x = vx[fast_vx]
	fast_y = vy[fast_vy]

	rx = (fast_x - v_max)/fast_x
	ry = (fast_y - v_max)/fast_y

	vx[fast_vx] = v_max * np.sign(fast_x) * (1 - rx)
	vy[fast_vy] = v_max * np.sign(fast_y) * (1 - ry)

	return vx, vy

def vmax_zero(vx, vy, v_max):
	vx[np.abs(vx) > v_max] = 0
	vy[np.abs(vy) > v_max] = 0

	return vx, vy

def check_max(x,xmax):
	y = x[np.abs(x) > xmax]
	if np.alen(y) > 0:
		print('max exceeded')

def speedup(vx):
	sx = 1/(1+np.exp(np.abs(vx)))
	vx += np.sign(vx) * sx
	return vx

def force_drift(f):

	xi = 0.01 * np.random.normal(0,1,np.shape(f))
	fn = f * (1 + xi) 
	return fn
