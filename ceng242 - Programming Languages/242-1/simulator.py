#!/usr/bin/python
__author__ = "alper & huseyin"
""" this script draws grids from the output of the1.hs saves them in a temp folder and deletes them after generating gif file. If the width and the height of grid is too big then you may need to adjust cellSize parameter in order to get better visualization. Other than that do NOT modify this file."""
import sys
from PIL import Image
from PIL import ImageDraw
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import cv2
import os
import shutil
import time
import datetime

#parameters
padding = 1
cellSize = 20

#colors
black = (0, 0, 0)
white = (255, 255, 255)

frames = []

def get_timestamp():
	return str(datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d_%H:%M:%S'))

def read_out_file(input_file):
    grids = []
    with open(input_file, 'r') as f:
        for line in f:
            grid = eval(line.strip())
            grids.append(grid)
    return grids
    
def load_all_images(dir):
 
    imgs = []
    for file_name in sorted(os.listdir(dir), key = lambda x:int(x.split('.')[0])):
        if os.path.splitext(file_name)[-1] == '.jpg':
 
            img = cv2.cvtColor(cv2.imread(os.path.join(dir, file_name)), cv2.COLOR_BGR2RGB)
            imgs.append(img)
 
    return imgs
 
def build_gif(imgs, filename, save_gif=True, title=''):
 
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.set_axis_off()
 
    ims = map(lambda x: (ax.imshow(x), ax.set_title(title)), imgs)
 
    im_ani = animation.ArtistAnimation(fig, ims, interval=400, repeat_delay=0, blit=False)
 
    if save_gif:
        im_ani.save(filename + '.gif', writer='imagemagick')
 
    return im_ani

if ( len(sys.argv) != 3 ):
    print "usage: ./simulator <input-filename> <output-filename>"
    sys.exit(1)
else:
    input_filename = sys.argv[1]
    output_filename = sys.argv[2]
    grids = read_out_file(input_filename)
    	
    
    numberOfRows = len(grids[0])
    numberOfColumns = len(grids[0][0])
    
    width = (cellSize + 2*padding) * numberOfColumns + padding
    height = (cellSize + 2*padding) * numberOfRows + padding

    i = 0
    folder = 'tmp' + get_timestamp()
    os.mkdir(folder)
    for grid in grids:    
        img = Image.new('RGB', (width, height))
        draw = ImageDraw.Draw(img)
        draw.rectangle((0, 0, width, height), fill=black)
        rowPixel = padding
        for r in range(numberOfRows):
            columnPixel = padding
            for c in range(numberOfColumns):
                if grid[r][c] == "B":
                    draw.rectangle((columnPixel, rowPixel, columnPixel + cellSize, rowPixel + cellSize), fill=black, outline=black)
                else:
                    draw.rectangle((columnPixel, rowPixel, columnPixel + cellSize, rowPixel + cellSize), fill=white, outline=white)			
                columnPixel += 2*padding + cellSize
            rowPixel += 2*padding + cellSize
        del draw
        img.save(folder+ '/' + str(i) + '.jpg', 'JPEG')
        i += 1
        
    images = load_all_images(folder)
    gif = build_gif(images, output_filename)
    shutil.rmtree(folder)
    plt.show()
