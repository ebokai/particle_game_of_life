import time
import pyautogui
import moviepy.editor as mp
import os
from tqdm import tqdm

def capture_screenshot(screen_num, interval, total_frames, save_path):
    """
    Captures screenshots at regular intervals and saves them to a specified directory.

    Parameters:
    - screen_num: int, 1 for the first screen, 2 for the second screen.
    - interval: float, time in seconds between each screenshot.
    - total_frames: int, total number of screenshots to capture.
    - save_path: str, directory to save the screenshots.
    """

    # Ensure the save_path directory exists
    os.makedirs(save_path, exist_ok=True)

    # Get the screen size
    screen_width, screen_height = pyautogui.size()
    
    # Adjust the region to capture based on the selected screen
    if screen_num == 1:
        region = (0, 0, screen_width, screen_height)
    else:
        region = (screen_width, 0, screen_width * 2, screen_height)

    # Progress bar for capturing screenshots
    for screenshot_count in tqdm(range(total_frames), desc="Capturing Screenshots", unit="frame"):
        # Take the screenshot
        screenshot = pyautogui.screenshot(region=region)

        # Save the screenshot
        screenshot.save(f"{save_path}/screenshot_{screenshot_count:04d}.png")

        # Wait for the next interval
        time.sleep(interval)

def convert_frames_to_video(fps, save_path):
    """
    Converts a sequence of image frames to a video file.

    Parameters:
    - fps: int, frames per second for the output video.
    - save_path: str, directory containing the image frames.
    """

    output_folder = './render'  # Directory to save the rendered video
    os.makedirs(output_folder, exist_ok=True)

    print('Loading frames...')
    image_files = sorted([os.path.join(save_path, img) for img in os.listdir(save_path) if img.endswith(".png")])
    print('Frames loaded.')

    print('Creating and rendering video...')
    clip = mp.ImageSequenceClip(image_files, fps=fps)
    
    print('...rendering video')
    clip.write_videofile(output_folder+'/'+f'timelapse_test.mp4')
    print('...complete')

if __name__ == "__main__":
    screen_num = 1  # Change this to 2 if you want to capture the second screen
    interval = 0.5  # Interval in seconds between screenshots
    duration = 30  # Desired duration of the timelapse video in seconds
    fps = 30  # Target frame rate for the video
    total_frames = duration * fps  # Total number of frames to capture
    save_path = "./frames"  # Directory to save screenshots
    convert = True  # Flag to determine whether to convert frames to video

    # Capture screenshots
    capture_screenshot(screen_num, interval, total_frames, save_path)

    # Convert captured frames to video if flag is set
    if convert:
        convert_frames_to_video(fps, save_path)
