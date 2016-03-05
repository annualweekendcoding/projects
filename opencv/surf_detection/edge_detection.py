# Python Imaging Library (PIL)
#import Image
# OpenCV Library
import cv2
import time

marker1 = cv2.imread('marker1.png')
marker1 = cv2.cvtColor(marker1,cv2.COLOR_BGR2GRAY)


cv2.namedWindow('camera')

capture = cv2.VideoCapture(0)


while 1:
    ret, img = capture.read()
    img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

    img_disp = img

    cv2.rectangle(img_disp,top_left, bottom_right, 255, 2)


    # Bild im Fenster anzeigen
    cv2.imshow('camera',img_disp)
    k = cv2.waitKey(10)
    if chr(k & 255) == 'q':
        break
