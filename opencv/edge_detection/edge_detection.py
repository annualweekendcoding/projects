import cv2
import time
import numpy as np
import zbar
import Image

picam = False
if picam:
    from picamera import PiCamera
    from picamera.array import PiRGBArray


def nothing(*arg):
    pass

marker1 = cv2.imread('marker1.png')
marker1 = cv2.cvtColor(marker1, cv2.COLOR_BGR2GRAY)

cv2.namedWindow('camera')
# cv2.createTrackbar('thres', 'camera', 118, 255, nothing)
# cv2.createTrackbar('clip', 'camera', 8, 10, nothing)
# cv2.createTrackbar('tiles', 'camera', 8, 16, nothing)

resolution = (1024, 768)
if picam:
    camera = PiCamera(resolution, framerate=10)
    print camera.resolution
else:
    camera = cv2.VideoCapture(0)
    camera.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, resolution[0])
    camera.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, resolution[1])
    camera.set(cv2.cv.CV_CAP_PROP_FPS, 10)
    # camera.set(15, -8.0)
time.sleep(0.1)

scanner = zbar.ImageScanner()
scanner.parse_config('enable')

count = 0
while 1:
    if picam:
        rawCapture = PiRGBArray(camera)
        camera.capture(rawCapture, use_video_port=True, format='bgr')
        img = rawCapture.array
        ret = img is not None
    else:
        camera.grab()
        ret, img = camera.retrieve()
    if ret:
        img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        img_disp = img.copy()

        img = cv2.GaussianBlur(img, (3, 3), 0)

        clahe = cv2.createCLAHE(2, (8, 8))
        img = clahe.apply(img)

        # retval, img = cv2.threshold(img, thres, 255, cv2.THRESH_BINARY_INV)
        img = cv2.Canny(img, 100, 200)

        contours, hierarchy = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        polys = []
        polyIndex = []
        for i in xrange(len(contours) - 1):
            # Darf keinen Nachbarn aber muss (mindestens) ein Kind haben
            iChild = hierarchy[0][i][2]
            if hierarchy[0][i][0] < 0 and hierarchy[0][i][1] < 0 and iChild >= 0:
                contour = contours[i]
                # moments = cv2.moments(contour)
                # cx = int(moments['m10'] / moments['m00'])
                # cy = int(moments['m01'] / moments['m00'])
                poly = cv2.approxPolyDP(contour, 3, True)
                if len(poly) == 4:
                    polyArea = cv2.contourArea(poly)
                    if cv2.isContourConvex(poly) and polyArea > 100:
                        child = cv2.approxPolyDP(contours[iChild], 3, True)
                        childArea = cv2.contourArea(child)
                        if len(child) != 4 or childArea * 1.5 < polyArea:
                            polys.append(poly)
                            polyIndex.append(i)
        count = len(polys)

        # Aussortieren der inneren Konturen
        outerPolys = []
        for i in xrange(count):
            iParent = hierarchy[0][polyIndex[i]][3]
            hasOuterPoly = False
            while iParent >= 0:
                if iParent in polyIndex:
                    hasOuterPoly = True
                    break
                iParent = hierarchy[0][iParent][3]
            if not hasOuterPoly:
                outerPolys.append(polys[i])

        #img_disp = cv2.cvtColor(img_disp, cv2.COLOR_GRAY2BGR)
        img_black = np.zeros((resolution[1], resolution[0], 3), np.uint8)
        for poly in outerPolys:
            rect = cv2.boundingRect(poly)
            cutted = img_disp[rect[1]:rect[1] + rect[3], rect[0]:rect[0] + rect[2]]
            cutImage = Image.fromarray(cutted)
            cutcol = cv2.cvtColor(cutted, cv2.COLOR_GRAY2BGR)
            imgZbar = zbar.Image(rect[2], rect[3], 'Y800', cutImage.tostring())
            scanner.scan(imgZbar)
            for symbol in imgZbar:
                # transform = cv2.getPerspectiveTransform(poly, rect)
                # corrected = cv2.warpPerspective(cutcol, transform, (rect[2] * 2, rect[3] * 2))
                img_black[rect[1]:rect[1] + rect[3], rect[0]:rect[0] + rect[2]] = cutcol
                print 'decoded', symbol.type, 'symbol', '"%s"' % symbol.data

        cv2.drawContours(img_black, outerPolys, -1, (255, 255, 255))

        cv2.imshow('camera', cv2.resize(img_black, (320, 240)))
    # endregion
    k = cv2.waitKey(10)
    if chr(k & 255) == 'q':
        break

cv2.destroyAllWindows()