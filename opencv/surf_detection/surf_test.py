import cv2

cv2.namedWindow('camera')
#cv2.namedWindow('marker1')
#cv2.namedWindow('marker2')

marker1 = cv2.imread('marker1.png')
marker1 = cv2.cvtColor(marker1,cv2.COLOR_BGR2GRAY)
marker2 = cv2.imread('marker2.png')
marker2 = cv2.cvtColor(marker2,cv2.COLOR_BGR2GRAY)

surf = cv2.SURF()
surf.hessianThreshold = 4000
surf.extended = True

detector = cv2.FeatureDetector_create("GFTT")
descriptor = cv2.DescriptorExtractor_create("SURF")

# detect keypoints
kp1 = detector.detect(marker1)
kp2 = detector.detect(marker2)
print '#keypoints in image1: %d, image2: %d' % (len(kp1), len(kp2))

#img_disp = cv2.drawKeypoints(marker1,kp1,None,(255,0,0),4)
#cv2.imshow('marker1',img_disp)

#img_disp = cv2.drawKeypoints(marker2,kp2,None,(255,0,0),4)
#cv2.imshow('marker2',img_disp)

# descriptors
#k1, d1 = descriptor.compute(marker1, kp1)
#k2, d2 = descriptor.compute(marker2, kp2)
k1, d1 = surf.detectAndCompute(marker1,None)
k2, d2 = surf.detectAndCompute(marker2,None)

matcher = cv2.BFMatcher() #FlannBasedMatcher()
    # pt1=(int(k1[m.queryIdx].pt[0]),int(k1[m.queryIdx].pt[1]))("FlannBased")

capture = cv2.VideoCapture(0)
while 1:
    ret, img = capture.read()
    img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    kp, d = surf.detectAndCompute(img,None)
    #kp = detector.detect(img)
    #k, d = descriptor.compute(img, kp)
    img_disp = cv2.drawKeypoints(img,kp,None,(255,0,0),4)

    matches = matcher.match(d1,d)
    dist = [m.distance for m in matches]
    # visualize the matches
    if min(dist)<0.1 :
        print '#matches:', len(matches), 'distance: min-mean-max: %.3f' % min(dist), '-%.3f' % (sum(dist) / len(dist)), '-%.3f' % max(dist)

    #matches = matcher.knnMatch(d1,d,6)
    #for match in matches

    cv2.imshow('camera',img_disp)
    k = cv2.waitKey(10)
    if chr(k & 255) == 'q':
        break
