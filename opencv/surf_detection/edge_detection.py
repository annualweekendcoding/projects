# Python Imaging Library (PIL)
#import Image
# OpenCV Library
import cv2
import time
import numpy as np
import time

#marker1 = cv2.imread('marker1.png')
#marker1 = cv2.cvtColor(marker1,cv2.COLOR_BGR2GRAY)

cv2.namedWindow('camera')

capture = cv2.VideoCapture(0)
capture.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,1024)
capture.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,768)
time.sleep(2)

while 1:
    ret, img = capture.read()
    img = cv2.GaussianBlur(img,(3,3),0)
    img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    #img = cv2.equalizeHist(img)

    #clahe = cv2.createCLAHE(2.0, (4,4))
    #img = clahe.apply(img)

    img_disp = img.copy()
    #img_disp = img.copy()

    img = cv2.Canny(img,50,150,3)
    #img_disp = img.copy()

    #ret, img = cv2.threshold(img,128,255,cv2.THRESH_BINARY_INV)
    contours, hierarchy = cv2.findContours(img,cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    polys1 = []
    polys = []
    for i in xrange (0,len(contours)-1):
        poly = cv2.approxPolyDP(contours[i], 3, True)
        polyArea = cv2.contourArea(poly)
        if len(poly) == 4 and polyArea>100:
            # Das Polygon ist ein Rechteck und hat eine Mindestgroesse
            polys1.append(poly)
            iChild = hierarchy[0][i][2]
            if iChild >= 0:
                # Es muss ein Child zu dem Polygon geben (Sonst ist nichts in dem Rechteck)
                child = cv2.approxPolyDP(contours[iChild], 3, True)
                childArea = cv2.contourArea(child)
                if len(child) != 4 or childArea*1.1 < polyArea:
                    # Das Child ist kein Rechteck oder es ist deutlich kleiner als das Polygon
                    zi = i
                    # aus der Hierarchie maximal 4 mal auftauchen
                    # Wenn dabei ein Rechteck gefunden wird welches in der Groesse passt
                    # Dann ist das Polygon gueltig
                    for z in range(1,4):
                        zi = hierarchy[0][zi][3]
                        if zi < 0: break
                        parent = cv2.approxPolyDP(contours[zi], 3, True)
                        parentArea = cv2.contourArea(parent)
                        if len(parent) == 4 and polyArea * 1.1 < parentArea < 1.5 * polyArea:
                            polys.append(poly)
                            break

    if len(polys)>0:
        print len(polys1), len(polys)
        #for poly in polys:
        #    print cv2.contourArea(poly), poly

    img_disp = cv2.cvtColor(img_disp,cv2.COLOR_GRAY2BGR)
    cv2.drawContours(img_disp,polys1,-1,(255,0,0))
    cv2.drawContours(img_disp,polys,-1,(0,255,0))

    #for contour in contours :
    #    co


    #cv2.rectangle(img_disp,top_left, bottom_right, 255, 2)


    # Bild im Fenster anzeigen
    cv2.imshow('camera',img_disp)
    k = cv2.waitKey(10)
    if chr(k & 255) == 'q':
        break
