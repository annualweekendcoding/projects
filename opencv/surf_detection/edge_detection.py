# Python Imaging Library (PIL)
import Image
# OpenCV Library
import cv2
import numpy as np
import time
import zbar
import argparse
import os

parser = argparse.ArgumentParser(description='Bilderkennung von QR-Codes in Rechtecken.')
parser.add_argument('-nx', '--no-x', action='store_true', help='X-Display ausschalten')
parser.add_argument('-np', '--no-picam', action='store_true', help='Pi-Camera nicht verwenden')
parser.add_argument('-uv', '--use-video', action='store_true', help='Pi-Camera nicht verwenden')
parser.add_argument('-x', '--width', default=640, type=int, help='X-Aufloesung der Kamera')
parser.add_argument('-y', '--height', default=480, type=int, help='Y-Aufloesung der Kamera')
parser.add_argument('-f', '--framerate', default=10, type=int, help='Framerate der Kamera')

args = parser.parse_args()

use_display = not args.no_x

if args.no_picam or True:
    picam = False
else:
    picam = os.uname()[4].startswith('arm')

if picam:
    from picamera import PiCamera
    from picamera.array import PiRGBArray

if use_display: cv2.namedWindow('camera')

resolution = (args.width, args.height)
if picam:
    camera = PiCamera(resolution=resolution, framerate=args.framerate)
    print "Resolution:", camera.resolution
else:
    camera = cv2.VideoCapture(0)
    camera.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, resolution[0])
    camera.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, resolution[1])
    camera.set(cv2.cv.CV_CAP_PROP_FPS, args.framerate)
    print "Resolution:", camera.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH), camera.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT)
    print "Framerate:", camera.get(cv2.cv.CV_CAP_PROP_FPS)

time.sleep(0.1)

scanner = zbar.ImageScanner()
# Scanner nur fuer QRCodes freischalten
scanner.parse_config('enable=0')
scanner.parse_config('qrcode.enable=1')

while 1:
    start_time=time.clock()

    k = cv2.waitKey(10)
    if chr(k & 255) == 'q':
        break

    if picam:
        rawCapture = PiRGBArray(camera)
        camera.capture(rawCapture, use_video_port=args.use_video, format='bgr')
        img = rawCapture.array
        ret = img is not None
    else:
        for i in range(1,5): ret, img = camera.read()

    if not ret: continue

    img = cv2.GaussianBlur(img,(3,3),0)
    img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    img_gray = img.copy()
    #img = cv2.equalizeHist(img)

    #clahe = cv2.createCLAHE(2.0, (4,4))
    #img = clahe.apply(img)

    if use_display: img_disp = img.copy()
    #img_disp = img.copy()

    img = cv2.Canny(img,50,150)
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
                        if len(parent) == 4 and polyArea * 1.1 < parentArea < polyArea * 1.5 :
                            polys.append(poly)
                            break

    if use_display:
        img_disp = cv2.cvtColor(img_disp,cv2.COLOR_GRAY2BGR)
        img_disp = np.zeros(((resolution[1]), (resolution[0]), 3), np.uint8)

    if len(polys)>0:
        print len(polys1), len(polys)

    for poly in polys:
        m = cv2.moments(poly)
        cx = int(m['m10']/m['m00'])
        cy = int(m['m01']/m['m00'])

        r = cv2.boundingRect(poly)
        print "Seitenverhaeltnis: ",float(r[2])/r[3]
        roi = img_gray[r[1]:r[1]+r[3], r[0]:r[0]+r[2]]
        pil_img = Image.fromarray(roi)
        zbar_img = zbar.Image(r[2], r[3], 'Y800', pil_img.tostring())
        # scan the image for barcodes
        scanner.scan(zbar_img)
        for symbol in zbar_img:
            print "Code:", symbol.data
            # Im Zentrum des Polygons ein roter Punkt
            if use_display: cv2.circle(img_disp,(cx,cy),5,(0,0,255),-1)

        if use_display:
            roi_color = cv2.cvtColor(roi,cv2.COLOR_GRAY2BGR)
            img_disp[r[1]:r[1]+r[3], r[0]:r[0]+r[2]] = roi_color


    if use_display:
        cv2.drawContours(img_disp,polys1,-1,(255,0,0))
        cv2.drawContours(img_disp,polys,-1,(0,255,0))
        img_disp = cv2.resize(img_disp, (int(resolution[0] / 2), int(resolution[1] / 2)))
        # Bild im Fenster anzeigen
        cv2.imshow('camera',img_disp)

    print "Zykluszeit: ", time.clock()-start_time

if use_display: cv2.destroyAllWindows()