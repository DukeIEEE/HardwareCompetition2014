#resizes all images
import os
import cv2

SIZE = (640, 480)
PATH = '.'
IMAGE_EXT = '.jpg'

imgfiles = [f for f in os.listdir(PATH) if os.path.isfile(os.path.join(PATH,f)) and f.endswith(IMAGE_EXT)]
for f in imgfiles:
  img = cv2.imread(f)
  img = cv2.resize(img, SIZE)
  cv2.imwrite(f, img)