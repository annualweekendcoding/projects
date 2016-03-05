#!/usr/bin/python

import cv2 as cv
from pymodbus.client.sync import ModbusSerialClient as ModbusClient

client = ModbusClient(method="rtu", port="/dev/ttyUSB0", stopbits=1, bytesize=8, parity='N', baudrate= 9600,
                      timeout=0.018)
# connect to the serial modbus server
connection = client.connect()
print connection

slave = 0x01

cv.namedWindow("camera", 1)
capture = cv.VideoCapture()
capture.open(0)
if not capture.isOpened():
    print 'nicht offen'
    exit()

faceCascade = cv.CascadeClassifier("/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml")

# width = None
# height = None
width = 320
height = 240
deviation = 5
factor = 0.1
servo_position = 512

if width is None:
    width = int(capture.get(capture.CV_CAP_PROP_FRAME_WIDTH))
else:
    capture.set(cv.cv.CV_CAP_PROP_FRAME_WIDTH, width)

if height is None:
    height = int(capture.get(capture.CV_CAP_PROP_FRAME_HEIGHT))
else:
    capture.set(cv.cv.CV_CAP_PROP_FRAME_HEIGHT, height)


def detect(face_image, face_cascade):
    global servo_position
    min_size = (20, 20)
    image_scale = 2
    haar_scale = 1.2
    min_neighbors = 2
    haar_flags = cv.cv.CV_HAAR_SCALE_IMAGE

    # Allocate the temporary images
    # gray = cv.CreateImage((face_image.width, face_image.height), 8, 1)
    # small_image = cv.CreateImage((cv.Round(face_image.width / image_scale), cv.Round(face_image.height / image_scale)),
    #                             8, 1)

    # Convert color input image to grayscale
    gray = cv.cvtColor(face_image, cv.COLOR_BGR2GRAY)

    # Scale input image for faster processing
    small_image = cv.resize(gray, (int(round(face_image.shape[1] / image_scale)), int(round(face_image.shape[0] / image_scale))),
                            interpolation=cv.INTER_LINEAR)

    # Equalize the histogram
    small_image = cv.equalizeHist(small_image)

    # Detect the faces
    faces = face_cascade.detectMultiScale(small_image, haar_scale, min_neighbors, haar_flags, min_size)

    # If faces are found
    largest_area = 0
    face_x_position = -1
    if len(faces) > 0:
        for (x, y, w, h) in faces:
            # the input to cv.HaarDetectObjects was resized, so scale the
            # bounding box of each face and convert it to two CvPoints
            pt1 = (int(x * image_scale), int(y * image_scale))
            pt2 = (int((x + w) * image_scale), int((y + h) * image_scale))
            area = w * h
            if area > largest_area:
                largest_area = area
                face_x_position = pt1[0] + ((pt2[0] - pt1[0]) / 2)
            cv.rectangle(face_image, pt1, pt2, (0, 0, 255), 3, 8, 0)

        relativ_x_position = (((width / 2) - face_x_position) * 1024) / width
        if abs(relativ_x_position) > ((width * deviation) / 100):
            servo_position += int(relativ_x_position * factor)
            cv.circle(face_image, (face_x_position, int(height / 2)), 10, (255, 0, 0), 3, 8, 0)
            if servo_position < 0:
                servo_position = 0
            elif servo_position > 1024:
                servo_position = 1024
            print str(servo_position)
        client.write_register(0x00, servo_position, unit=slave)

    return face_image


while True:
    ret, img = capture.read()

    image = detect(img, faceCascade)

    cv.imshow("camera", image)
    k = cv.waitKey(10)
    if chr(k & 255) == 'q':
        client.close()
        break
