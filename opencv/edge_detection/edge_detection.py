import cv2
import time


def nothing(*arg):
    pass

marker1 = cv2.imread('marker1.png')
marker1 = cv2.cvtColor(marker1, cv2.COLOR_BGR2GRAY)

cv2.namedWindow('camera')
# cv2.createTrackbar('thres', 'camera', 118, 255, nothing)
# cv2.createTrackbar('clip', 'camera', 8, 10, nothing)
# cv2.createTrackbar('tiles', 'camera', 8, 16, nothing)

capture = cv2.VideoCapture(0)
capture.set(3, 640)
capture.set(4, 480)
time.sleep(2)
# capture.set(15, -8.0)

count = 0
while 1:
    ret, img = capture.read()
    img_disp = img.copy()
    # thres = cv2.getTrackbarPos('thres', 'camera')
    # clip = cv2.getTrackbarPos('clip', 'camera')
    # tiles = cv2.getTrackbarPos('tiles', 'camera')
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img = cv2.GaussianBlur(img, (3, 3), 0)
    clahe = cv2.createCLAHE(2, (8, 8))
    img = clahe.apply(img)
    # retval, img = cv2.threshold(img, thres, 255, cv2.THRESH_BINARY_INV)
    img = cv2.Canny(img, 100, 200)

    contours, hierarchy = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    polys = []
    polyIndex = []
    for i in xrange(len(contours) - 1):
        # if hierarchy[0][i][3] >= 0:
        #     poly = cv2.approxPolyDP(contours[i], 3, 1)
        #     if len(poly) == 4 and cv2.isContourConvex(poly):
        #         iParent = hierarchy[0][i][3]
        #         parentPoly = cv2.approxPolyDP(contours[iParent], 3, 1)
        #         if len(parentPoly) == 4:
        #             polyArea = cv2.contourArea(poly)
        #             parentArea = cv2.contourArea(parentPoly)
        #             if parentArea < polyArea * 2.0 and parentArea > polyArea * 1.1:
        #                 polys.append(poly)
        #                 polyIndex.append(i)
        contour = contours[i]
        # moments = cv2.moments(contour)
        # cx = int(moments['m10'] / moments['m00'])
        # cy = int(moments['m01'] / moments['m00'])
        poly = cv2.approxPolyDP(contour, 3, True)
        iChild = hierarchy[0][i][2]
        if len(poly) == 4 and cv2.isContourConvex(poly) and iChild >= 0:
            hasNear = False
            while not hasNear and iChild >= 0:
                childContour = contours[iChild]
                child = cv2.approxPolyDP(childContour, 3, True)
                if len(child) == 4 and cv2.isContourConvex(child):
                    polyArea = cv2.contourArea(poly)
                    childArea = cv2.contourArea(child)
                    if childArea * 2.0 > polyArea: hasNear = True
                iChild = hierarchy[0][iChild][0]
            if not hasNear:
                polys.append(poly)
                polyIndex.append(i)
    count = len(polys)
    print count

    cv2.drawContours(img_disp, polys, -1, (255, 0, 0))

    cv2.imshow('camera', img_disp)
    k = cv2.waitKey(10)
    if chr(k & 255) == 'q':
        break

cv2.destroyAllWindows()