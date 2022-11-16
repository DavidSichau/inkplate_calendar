import os
import cairosvg

size=24

for file in os.listdir('.'):
    if os.path.isfile(file) and file.endswith(".svg"):
        name = file.split('.svg')[0]
        cairosvg.svg2png(url=name+'.svg',write_to=name+'.png', output_width=size, output_height=size)
