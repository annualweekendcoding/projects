# Python Imaging Library (PIL)
#import Image
# OpenCV Library
import cv2
import time

import numpy as np

#import pygame

DRAW_DESC = 0
DRAW_CORRESPONDANCE = 0
DRAW_BORDERS = 1

FIT_DESC_TH = 3
COSTDESC = 0.2

# Load Markers Descriptors
#IMG0 = np.load('./Beispiel_desc.npy')
ALLIMG = []

#KP0 = np.load('./Beispiel_kp.npy')
ALLKP = []

MARKER_NAMES = ['Beispiel']
SCALES = [3., 3.3, 4.1, 3.]

CORNERS = [[(140, 139), (576, 139), (576, 469), (140, 469)],
           [(190.5, 193.5), (637.5, 190.5), (639, 414), (192.2, 412)],
           [(168, 172.5), (673.5, 169.5), (682.5, 675), (175.5, 674.0)],
           [(31, 31), (580, 31), (585, 580), (31, 580)]
]

K = np.load('./Intrinsic.npy')


# Given two vectors, return the euclidean distance
def euclidDist(v1, v2):
    acum = 0
    for i in range(0, len(v1)):
        acum += np.power(v1[i] - v2[i], 2)

    return np.sqrt(acum)


def euclidDist2(v1, v2):
    return np.dot((v1 - v2).T, (v1 - v2))


def knn(query, data, k=1):
    diferencia = data - query
    dists = (diferencia ** 2).sum(1)  # Suma columnas
    indices = np.argsort(dists)[:k]
    return np.sqrt(dists[indices]), indices


def getHistogram(descriptors, keypoints):
    # Restart of histogram values
    histogram = [0, 0, 0, 0]
    #For each descriptor compare with the given marker descriptors

    #Histogram for storing indexes of descriptors correspondances
    canonicalPoints = [[], [], [], []]

    capturePoints = [[], [], [], []]

    idx = 0
    for desc in descriptors:

        #Best descriptor distance
        bestDist = None

        bestIdx = None

        bestImage = None
        #Second best descriptor
        secondBestDist = None

        for img in range(0, len(ALLIMG)):


            dist, ind = knn(desc, ALLIMG[img], 2)

            #Calc the two most 

            if bestDist == None:
                bestDist = dist[0]
                bestIdx = ind[0]
                secondBestDist = dist[1]
                bestImage = img
                continue

            if dist[1] < bestDist:
                bestDist = dist[0]
                bestIdx = ind[0]
                secondBestDist = dist[1]
                bestImage = img

            elif dist[1] > bestDist and dist[1] < secondBestDist:
                secondBestDist = dist[0]

            if dist[0] < bestDist:
                bestDist = dist[0]
                bestIdx = ind[0]
                bestImage = img

            elif dist[0] > bestDist and dist[0] < secondBestDist:
                secondBestDist = dist[0]

        if not (bestDist < COSTDESC):
            #if not(bestDist < COSTDESC * secondBestDist):
            idx += 1
            continue

        ((x, y), laplacian, size, dir, hessian) = keypoints[idx]

        (xc, yc, dirc, sizec, hessianc) = ALLKP[bestImage][bestIdx]

        histogram[bestImage] += 1

        capturePoints[bestImage].append((x, y))
        canonicalPoints[bestImage].append((xc, yc))


        #Draw correspondances in image

        if DRAW_CORRESPONDANCE:
            radio = size * 1.2 / 9. * 2
            color = (0, 0, 255)

            #print "radioNew: ", int(radio)
            cv2.circle(img_display, color, (x, y), radio, 2)


            #Increment the index
        idx += 1


    #Return histogram
    return histogram, canonicalPoints, capturePoints


'Get homography: Ransac'


def getHomography(histogram, capturePoints, canonicalPoints):
    NH = 0

    orderIdx = np.argsort(histogram)

    bestMarker = orderIdx[len(orderIdx) - 1]
    secondBestMarker = orderIdx[len(orderIdx) - 2]

    H1 = None
    H2 = None
    bestRatio = None

    mat_canonical_points1 = None
    mat_canonical_points2 = None

    # Get RANSCAR homography for the best marker
    if histogram[bestMarker] >= 4:
        H1 = cv2.CreateMat(3, 3, cv2.CV_32FC1)

        mat_canonical_points1 = cv2.CreateMat(len(canonicalPoints[bestMarker]), 3, cv2.CV_32FC1)
        mat_capture_points1 = cv2.CreateMat(len(canonicalPoints[bestMarker]), 3, cv2.CV_32FC1)

        for i in range(0, len(canonicalPoints[bestMarker])):
            mat_canonical_points1[i, 0] = canonicalPoints[bestMarker][i][0]
            mat_canonical_points1[i, 1] = canonicalPoints[bestMarker][i][1]
            mat_canonical_points1[i, 2] = 1

            mat_capture_points1[i, 0] = capturePoints[bestMarker][i][0]
            mat_capture_points1[i, 1] = capturePoints[bestMarker][i][1]
            mat_capture_points1[i, 2] = 1

        cv2.findHomography(mat_canonical_points1, mat_capture_points1, H1, cv2.CV_RANSAC, 10)

        NH += 1



    #Get RANSCAR homography for the second best marker
    if histogram[secondBestMarker] >= 4:
        H2 = cv2.CreateMat(3, 3, cv2.CV_32FC1)
        mat_canonical_points2 = cv2.CreateMat(len(canonicalPoints[secondBestMarker]), 3, cv2.CV_32FC1)
        mat_capture_points2 = cv2.CreateMat(len(canonicalPoints[secondBestMarker]), 3, cv2.CV_32FC1)

        for i in range(0, len(canonicalPoints[secondBestMarker])):
            mat_canonical_points2[i, 0] = canonicalPoints[secondBestMarker][i][0]
            mat_canonical_points2[i, 1] = canonicalPoints[secondBestMarker][i][1]
            mat_canonical_points2[i, 2] = 1

            mat_capture_points2[i, 0] = capturePoints[secondBestMarker][i][0]
            mat_capture_points2[i, 1] = capturePoints[secondBestMarker][i][1]
            mat_capture_points2[i, 2] = 1

        cv2.findHomography(mat_canonical_points2, mat_capture_points2, H2, cv2.CV_RANSAC, 10)

        NH += 1

    THRESHOLD = 10

    bestH = None
    bestImage = None

    if (H1 != None):
        ##Calculare outliner and inliners for H1
        i1 = 0.
        o1 = 0.

        H1np = np.asarray(H1)

        for i in range(0, len(canonicalPoints[bestMarker])):


            tempPoint = np.dot(H1np, np.asmatrix(mat_canonical_points1[i]).T)

            dist = euclidDist([tempPoint[0] / tempPoint[2], tempPoint[1] / tempPoint[2]], capturePoints[bestMarker][i])

            if dist < THRESHOLD:
                i1 += 1.
            else:
                o1 += 1.

    if (H2 != None):
        ##Calculte outliners and inliners for H2
        i2 = 0.
        o2 = 0.

        H2np = np.asarray(H2)

        for i in range(0, len(canonicalPoints[secondBestMarker])):

            tempPoint = np.dot(H2np, np.asmatrix(mat_canonical_points2[i]).T)

            dist = euclidDist([tempPoint[0] / tempPoint[2], tempPoint[1] / tempPoint[2]],
                              capturePoints[secondBestMarker][i])

            if dist < THRESHOLD:
                i2 += 1.
            else:
                o2 += 1.


                #print "I/0: "+str(i1)+", "+str(o1)+", "+str(i2)+", "+str(o2)

        #Compare ratios of inliers and outliers
        if (i1 / (i1 + o1) < i2 / (i2 + o2)):
            bestH = H2
            bestImage = secondBestMarker
            bestRatio = i1 / (i1 + o1)
        else:
            bestH = H1
            bestImage = bestMarker
            bestRatio = i2 / (i2 + o2)

    if H1 != None and H2 == None:
        bestH = H1
        bestImage = bestMarker
        bestRatio = i1 / (i1 + o1)

    return bestH, bestImage, NH, bestRatio


# Create the histogram of descriptor correspondance per marker
histogram = [0, 0, 0, 0]

# INITIALIZE camera
camera = cv2.VideoCapture(0)
cv2.namedWindow('camera')

img = camera.read()

surf = cv2.SURF()
surf.hessianThreshold = 2000
# surf.extended = True

while 1:
    startTime = time.time()
    key = chr(cv2.waitKey(10) & 255)
    if key=='q': break

    # CAPTURE IMAGE
    ret, img = camera.read()
    #img_rgb = cv2.CvtColor(img, cv2.CV_BGR2RGB)
    grayscale = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img_display = img

    #descriptors =[]
    # try:

    keypoints, descriptors = surf.detectAndCompute(grayscale,None)
    #cv2.extractSURF(grayscale, None, cv2.CreateMemStorage(), (0, 2000, 3, 2))
    # DRAW KEYPOINT
    img_display = cv2.drawKeypoints(img_display,keypoints,None,(255,0,0),4)

    #Get histogram
    if len(descriptors)>0:
        histogram, canonicalPoints, capturePoints = getHistogram(descriptors, keypoints)

        H, bestImage, NH, bestRatio = getHomography(histogram, capturePoints, canonicalPoints)

        if H != None:

            Hn = np.asarray(H)

            if DRAW_BORDERS:
                points_to_print = []

                #Get corners from homography

                #CORNERS[bestImage] = CORNERS[bestImage]/SCALES[bestImage]

                for i in range(0, 4):
                    point = np.matrix([CORNERS[bestImage][i][0], CORNERS[bestImage][i][1], 1])

                    point = point
                    temp = np.dot(Hn, point.T)

                    point = [temp[0] / temp[2], temp[1] / temp[2]]

                    points_to_print.append(point)



                #Draw corners
                for i in range(0, len(points_to_print)):
                    cv2.circle(img_display, (0, 255, 0), (points_to_print[i][0], points_to_print[i][1]), 10, 0)

                    #Draw lines
                    cv2.lines(img_display, (0, 255, 0), 1, points_to_print, 4)


                #Draw image center
                cv2.circle(img_display, (0, 0, 0), (K[0, 2], K[1, 2]), 6, 0)

                #Get marker origin
                origin = np.dot(Hn, np.matrix([0, 0, 1]).T)

                cv2.circle(img_display, (0, 0, 0), (origin[0], origin[1]), 3, 0)

                cv2.line(img_display, (0, 0, 0), (K[0, 2], K[1, 2]), (origin[0], origin[1]), 2)

            invK = np.linalg.inv(K)
            lb = (np.linalg.norm(np.dot(invK, Hn[:, 0])) + np.linalg.norm(np.dot(invK, Hn[:, 1]))) / 2.0

            r1 = (1.0 / lb) * np.dot(invK, Hn[:, 0])
            t = (1.0 / lb) * np.dot(invK, Hn[:, 2])

            tx = t[0] / SCALES[bestImage]
            tz = t[2] / SCALES[bestImage]

            thetaR = np.arctan(-1 * r1[2] / r1[0]) * 180.0 / np.pi
            #




            #img_display.fill((255,0,0))


    # except Exception, e:
    #     print e
    #print len(keypoints)


    #Print time period in miliseconds
    endTime = time.time()
    print "Image period: %0.3f ms." % ((endTime - startTime) * 1000.0)

    #Draw Text
    if max(histogram) < FIT_DESC_TH:
        cv2.putText(img_display,"No Marker Found",(0, 10),cv2.FONT_HERSHEY_PLAIN,1,(0,0,255))
    else:
        #print "Marker found! Image: "+str(np.argmax(histogram,0))+"["+str(max(histogram))+"]"
        #print "Histogram: " +str(histogram)

        if H == None:
            bestImage = np.argmax(histogram, 0)

        cv2.putText(img_display,
            "Marker " + str(bestImage) + ": " + MARKER_NAMES[bestImage] + " [" + str(histogram[bestImage]) + "]",
            (0, 10),cv2.FONT_HERSHEY_PLAIN,1)

        if H != None:
            #print "homography: "+str(np.asarray(H))
            cv2.putText(img_display,str(NH) + " Homography found: " + str(bestRatio), (350, 10),cv2.FONT_HERSHEY_PLAIN,1,(0,0,255))
            cv2.putText(img_display,"Tx:%12.3f" % tx + " mm",(0, 70) ,cv2.FONT_HERSHEY_PLAIN,1,(0,0,255))
            cv2.putText(img_display,"Tz:%12.3f" % tz + " mm",(0, 90) ,cv2.FONT_HERSHEY_PLAIN,1,(0,0,255))
            cv2.putText(img_display,"Theta:%8.3f" % thetaR + " degrees",(0, 110) ,cv2.FONT_HERSHEY_PLAIN,1,(0,0,255))

    # ADD image to the work surface
    cv2.imshow('camera',img_display)
