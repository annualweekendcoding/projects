import cv2

cv2.namedWindow('camera')
cv2.namedWindow('img1')
cv2.namedWindow('img2')

img1 = cv2.imread('../kiste.jpg')
img2 = cv2.imread('../lampe.jpg')
img1 = cv2.cvtColor(img1,cv2.COLOR_BGR2GRAY)
img2 = cv2.cvtColor(img2,cv2.COLOR_BGR2GRAY)

detector = cv2.FeatureDetector_create("GFTT")
descriptor = cv2.DescriptorExtractor_create("SURF")

# detect keypoints
kp1 = detector.detect(img1)
kp2 = detector.detect(img2)
print '#keypoints in image1: %d, image2: %d' % (len(kp1), len(kp2))

img_disp = cv2.drawKeypoints(img1,kp1,None,(255,0,0),4)
cv2.imshow('img1',img_disp)

img_disp = cv2.drawKeypoints(img2,kp2,None,(255,0,0),4)
cv2.imshow('img2',img_disp)

# descriptors
k1, d1 = descriptor.compute(img1, kp1)
k2, d2 = descriptor.compute(img2, kp2)

matcher = cv2.BFMatcher() #FlannBasedMatcher()
    # pt1=(int(k1[m.queryIdx].pt[0]),int(k1[m.queryIdx].pt[1]))("FlannBased")

# match the keypoints
matches = matcher.match(d1,d2)

# visualize the matches
print '#matches:', len(matches)
dist = [m.distance for m in matches]

print 'distance: min: %.3f' % min(dist)
print 'distance: mean: %.3f' % (sum(dist) / len(dist))
print 'distance: max: %.3f' % max(dist)

capture = cv2.VideoCapture(0)
surf = cv2.SURF()
surf.hessianThreshold = 2000
surf.extended = True
while 1:
    ret, img = capture.read()
    img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    # kp, des = surf.detectAndCompute(img,None)
    kp = detector.detect(img)
    k, d = descriptor.compute(img, kp)
    img_disp = cv2.drawKeypoints(img,kp,None,(255,0,0),4)

    matches = matcher.match(d2,d)
    dist = [m.distance for m in matches]
    # visualize the matches
    print '#matches:', len(matches)
    print 'distance: min: %.3f' % min(dist)
    print 'distance: mean: %.3f' % (sum(dist) / len(dist))
    print 'distance: max: %.3f' % max(dist)

    cv2.imshow('camera',img_disp)
    k = cv2.waitKey(10)
    if chr(k & 255) == 'q':
        break
