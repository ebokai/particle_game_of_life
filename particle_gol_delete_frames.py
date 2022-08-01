import os 

# specify image folder here
image_folder='c:/users/hp/desktop/frames'

for img in os.listdir(image_folder): 
	if img.endswith(".png"):
		os.remove(image_folder+'/'+img)