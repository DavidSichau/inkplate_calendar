import os
import cairosvg
import subprocess


sizes = [16, 24,32,64,128,256]
for size in sizes:
    print(size)

    for file in os.listdir('.'):
        if os.path.isfile(file) and file.endswith(".svg"):
            name = file.split('.svg')[0]
            if not os.path.exists(str(size)):
                os.mkdir(str(size))
            cairosvg.svg2png(url=name+'.svg',write_to=str(size)+"/"+name+'.png', output_width=size, output_height=size)
            # convert wi-cloud.png -background white -alpha remove -alpha off white.png
            # if not os.path.exists(str(size)):
            #     os.mkdir(str(size))
            # os.system("convert "+name+".png -background white -alpha remove -alpha off "+str(size)+"/"+name+".png")

