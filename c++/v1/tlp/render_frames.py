import moviepy
import os
import time

import moviepy.video.io.ImageSequenceClip
image_folder = './frames'
output_folder = './render'
fps = 30

print('...loading frames')
image_files = [image_folder+'/'+img for img in os.listdir(image_folder) if img.endswith(".png")]
print('...complete')

print('...creating sequence')
clip = moviepy.video.io.ImageSequenceClip.ImageSequenceClip(image_files, fps=fps)
print('...complete')

print('...rendering video')
clip.write_videofile(output_folder+'/'+f'timelapse_test.mp4')
print('...complete')