# Python Imaging Library (PIL)
#import Image
# OpenCV Library
import cv2
import time

marker1 = cv2.imread('marker1.png')
marker1 = cv2.cvtColor(marker1,cv2.COLOR_BGR2GRAY)
w, h = marker1.shape[::-1]

cv2.namedWindow('camera')

capture = cv2.VideoCapture(0)

# All the 6 methods for comparison in a list
methods = ['cv2.TM_CCOEFF', 'cv2.TM_CCOEFF_NORMED', 'cv2.TM_CCORR',
           'cv2.TM_CCORR_NORMED', 'cv2.TM_SQDIFF', 'cv2.TM_SQDIFF_NORMED']

while 1:
    ret, img = capture.read()
    img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    method = eval(methods[1])
    res = cv2.matchTemplate(img,marker1,method)
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    # If the method is TM_SQDIFF or TM_SQDIFF_NORMED, take minimum
    if method in [cv2.TM_SQDIFF, cv2.TM_SQDIFF_NORMED]:
        top_left = min_loc
    else:
        top_left = max_loc
    bottom_right = (top_left[0] + w, top_left[1] + h)

    img_disp = res

    cv2.rectangle(img_disp,top_left, bottom_right, 255, 2)


    # Bild im Fenster anzeigen
    cv2.imshow('camera',img_disp)
    k = cv2.waitKey(10)
    if chr(k & 255) == 'q':
        break
