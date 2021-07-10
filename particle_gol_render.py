import moviepy
import os
import time

import moviepy.video.io.ImageSequenceClip
image_folder='c:/users/hp/desktop/frames'
output_folder='c:/users/hp/desktop/frames/render'
fps = 24

image_files = [image_folder+'/'+img for img in os.listdir(image_folder) if img.endswith(".png")]

clip = moviepy.video.io.ImageSequenceClip.ImageSequenceClip(image_files, fps=fps)
clip.write_videofile(output_folder+'/'+f'pgol_{time.time():.0f}.mp4')