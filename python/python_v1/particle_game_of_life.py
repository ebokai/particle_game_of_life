import numpy as np 
import matplotlib.pyplot as plt 
import matplotlib.animation as anim 
import colorsys as colsys

np.set_printoptions(precision=3)
plt.rcParams['axes.facecolor'] = '#000000'

def block_matrix(p0,p_std,pos=False):
	"""
	generates block matrix (used for assigning 
	parameters to groups of particles)
	"""
	m = np.random.normal(p0,p_std,(groups,groups))
	m = m.repeat(particles_per_group,axis=1).repeat(particles_per_group,axis=0)

	if pos:
		return np.abs(m)
	else:
		return m

def bound(x,y):
	"""
	periodic boundaries
	"""
	x[x>=x_width*lim] -= 2*x_width*lim
	x[x<-x_width*lim] += 2*x_width*lim
	y[y>=lim] -= 2*lim
	y[y<-lim] += 2*lim

	return x,y


def attract(x,y,vx,vy):
	"""
	calculate forces between particles 
	"""

	dx = x.reshape(total_particles,1) - x
	dy = y.reshape(total_particles,1) - y
	dx,dy = bound(dx,dy)

	r = np.sqrt(dx*dx + dy*dy)
	f = np.zeros((total_particles,total_particles))
	i0 = np.where(r < separation)
	i1 = np.where((r > separation) & (r < (separation+force_radius/2)))
	i2 = np.where((r > separation+force_radius/2) & (r < separation+force_radius)) 

	f[i0] = rep_force[i0]*(separation[i0]-r[i0])
	f[i1] = forces[i1] * (r[i1]-separation[i1]) 
	f[i2] = -forces[i2] * (r[i2]-(separation[i2]+force_radius[i2]))
	
	np.fill_diagonal(f,0) # no self-force

	# increase force at long range
	f *= close_range_factor + (dist_range_factor-close_range_factor) * (r/lim)

	a = np.arctan2(dy,dx)

	# add contributions from all particles
	fx = np.sum(f * np.cos(a), axis=0)
	fy = np.sum(f * np.sin(a), axis=0)

	return fx,fy

def friction(vx,vy):
	"""
	friction and thermal forces
	"""

	v = np.sqrt(vx*vx + vy*vy)
	a = np.arctan2(vy,vx)
	fx = friction_coefficient * v * np.cos(a) + np.random.normal(0,temperature,total_particles)
	fy = friction_coefficient * v * np.sin(a) + np.random.normal(0,temperature,total_particles)

	return fx,fy

def init():
	x = np.zeros((1,2))
	particles.set_offsets(x)
	particles_area.set_offsets(x)
	return particles, particles_area,

def animate(i):

	global x,y,vx,vy
	
	x,y = bound(x,y)
	fx,fy = attract(x,y,vx,vy)
	ffx,ffy = friction(vx,vy)

	x += vx * dt
	y += vy * dt 
	vx += (-fx-ffx) * dt
	vy += (-fy-ffy) * dt 

	xy = np.vstack((x,y)).T

	particles.set_offsets(xy)
	particles_area.set_offsets(xy)

	return particles, particles_area,

# global settings
dt = 0.01
lim = 60
x_width = 1
groups = 3
particles_per_group = 200
total_particles = groups * particles_per_group

# parameters
repelling_force = 60 # force active if r < separation radius
temperature = 5 # controls random fluctuations in particle velocities
friction_coefficient = 90 
separation_radius = 10 #mean separation radius
interaction_radius = 25 # mean interaction radius
force_strength = 10 # inter-particle force strength
close_range_factor = 1 # force strength multiplier at r=0
dist_range_factor = 5 # force strength multiplier at r=lim
deviation = 0.1 # spread in group parameters
seed_range = 0.9 # initial position spread


# parameter matrices 
force_radius = block_matrix(interaction_radius,interaction_radius*deviation,True)
separation = block_matrix(separation_radius,separation_radius*deviation,True)
forces = block_matrix(0,force_strength)
rep_force = block_matrix(repelling_force,repelling_force*deviation,True)



# group properties 
cs,ss,sa = [],[],[]
ppg = particles_per_group
for g in range(groups):

	fr = np.abs(force_radius[0,g*ppg])/np.max(force_radius)
	fs = np.abs(forces[0,g*ppg])/np.max(np.abs(forces))
	sep = np.abs(separation[0,g*ppg])/np.max(separation)
	rep = np.abs(rep_force[0,g*ppg])/np.max(rep_force)
	fd = np.abs(forces[g*ppg,g*ppg])/np.max(np.abs(forces))

	size = 20 + 50 * fs 

	fc = (forces[0,g*ppg]-np.min(forces))/(np.max(forces)-np.min(forces))

	color = colsys.hsv_to_rgb(fc,0.3+0.7*fd,0.5*(1+fs))
	for p in range(particles_per_group):
		cs.append(color)
		ss.append(size)
		sa.append(2*fr*15*size)

# initialization
x = np.random.uniform(-lim*seed_range,lim*seed_range,total_particles)
y = np.random.uniform(-lim*seed_range,lim*seed_range,total_particles)
vx = np.random.uniform(-1,1,total_particles)
vy = np.random.uniform(-1,1,total_particles)



fig,ax = plt.subplots(figsize=(14,7),facecolor='k')
# plt.title(titlestring)
ax.set_aspect('equal')
plt.xlim(-x_width*lim,x_width*lim)
plt.ylim(-lim,lim)
particles = plt.scatter(x, y, marker='.', c=cs, s=ss)
particles_area = plt.scatter(x, y, marker='.', c=cs, s=sa, alpha=0.1, edgecolors=None, linewidths=0)

animation = anim.FuncAnimation(fig, animate, init_func=init, interval=0, blit=True)

plt.axis('off')
plt.tight_layout(pad=0.3)
plt.subplots_adjust(hspace=0.01, wspace=0.01)
# figManager = plt.get_current_fig_manager()
# figManager.window.showMaximized()
plt.show()