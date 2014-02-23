# runs through all image files in the directory and spits out a results.txt file
# that contains the target location

import os
import os.path
import sys
import cv2
import numpy as np

from util import read_results,write_results

OUTFILE = 'results.txt' #output file name
IMAGE_EXT = '.jpg' #extension for image files
PATH = '.' #path to search in

#check command line arguments
if len(sys.argv) > 1:
  PATH = sys.argv[1]
if len(sys.argv) > 2:
  OUTFILE = sys.argv[2]

data = {}
try:
  data = read_results(os.path.join(PATH,OUTFILE))
except IOError:
  pass #file does not exist yet
  
#grab all jpegs
imagefiles = [f for f in os.listdir(PATH) if os.path.isfile(os.path.join(PATH,f)) and f.endswith(IMAGE_EXT)]

#split into new and old images
newimages = [img for img in imagefiles if img not in data]
oldimages = [img for img in data if img not in newimages] #retain all data saved in results.txt by iterating over data, not imagefiles!
images = newimages + oldimages #put old images at the back

#setup mouse callback
def set_center(event,x,y,flags,param):
  global pos
  if event == cv2.EVENT_LBUTTONUP:
    pos = (x,y)

cv2.namedWindow('Image')
cv2.setMouseCallback('Image', set_center)    
    
pos = None
index = 0
if images[index] in data:
  pos = data[images[index]]
print("Displaying image {0} at {1}".format(index, images[index]))
while True:
  img = cv2.imread(os.path.join(PATH,images[index]))
  img_disp = np.copy(img)
  if pos is not None:
    cv2.circle(img_disp, pos, 3, (255, 0, 255), -1)
    data[images[index]] = pos
  cv2.imshow('Image',img_disp)
  
  key = cv2.waitKey(1) & 0xFF
  changeimg = False
  if key == ord('z'): #prev image
    index = index - 1
    if index < 0:
      index = len(images) - 1
      print("Last image")
    changeimg = True
  elif key == ord('x'): #next image
    index = index + 1
    if index >= len(images):
      index = 0
      print("First image")
    changeimg = True
  elif key == 27:
    break
    
  if changeimg:
    pos = None
    if images[index] in data:
      pos = data[images[index]]
    print("Displaying image {0} at {1}".format(index, images[index]))
cv2.destroyAllWindows()    

print("Saving data...")
write_results(os.path.join(PATH,OUTFILE), data)
