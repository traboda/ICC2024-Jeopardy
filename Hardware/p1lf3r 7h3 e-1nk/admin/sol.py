import numpy as np
from PIL import Image

width = 800
height = 480

export_file = "asia_icc.txt"

with open(export_file, 'r') as f:
    raw_data = f.readlines()

new_raw = [x.split(",") for x in raw_data]

black = []
red = []

for i in range(48000):
    black.append(new_raw[i][-2].strip())
    red.append(new_raw[48000+i][-2].strip())

binary_data_black = bytes(int(x, 16) for x in black)
binary_data_red = bytes(int(x, 16) for x in red)

binary_image_black = np.unpackbits(np.frombuffer(binary_data_black, dtype=np.uint8)).reshape((height, width))
binary_image_red = np.unpackbits(np.frombuffer(binary_data_red, dtype=np.uint8)).reshape((height, width))

image_black = Image.fromarray(255 * (1 - binary_image_black)).convert('L')
image_red = Image.fromarray(255 * (binary_image_red)).convert('L')

image_black.save("black.png")
print('[+] Generated Black Image')

image_red.save("red.png")
print('[+] Generated Red/Yellow Image')