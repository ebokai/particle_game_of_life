particle game of life

simulations of particles with asymmetric attraction/repulsion

# python
new version (corresponding to this video: https://www.youtube.com/watch?v=P1H3MM56DDo)

dependencies:
- moviepy (only for particle_gol_rander.py)
- colorsys

standard libraries (should be included with most python distributions):
- numpy
- matplotlib
- time
- os

----

  particle_gol_delete_frames.py: delete all rendered frames from output folder
  
  particle_gol_functions.py: simulation functions
  
  particle_gol_main.py: real-time rendering using matplotlib animation
  
  particle_gol_main_saveframes.py: off-line rendering to image files
  
  particle_gol_params.py: simulation parameters - separate file to allow sharing of parameter values between ..main.py and ..main_saveframes.py
  
  particle_gol_render.py: render frames to .mp4 (uses moviepy package)
  
  particle_gol_sim.py: simulation - called by ..main.py and ..main_saveframes.py

----

old version (corresponding to this video: https://www.youtube.com/watch?v=-c5XaC5-DXg)

  /old/pgol_old.py 
  
# C++
to be added
  
