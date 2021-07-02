import os 

image_folder='c:/your_output_frames_folder/frames'

for img in os.listdir(image_folder): 
	if img.endswith(".png"):
		os.remove(image_folder+'/'+img)
