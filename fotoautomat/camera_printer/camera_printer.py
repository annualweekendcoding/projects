#!/usr/bin/python2
# coding=UTF-8
# OpenCV Library
import cv2
import argparse
import os
import re
import platform
import time
from PIL import Image, ExifTags
import numpy as np
import random

raspberry=False
if platform.system()=='Linux':
    from subprocess import call
    import RPi.GPIO as GPIO
    # RPi.GPIO Layout verwenden (wie Pin-Nummern)
    GPIO.setmode(GPIO.BOARD)
    # Pin 18 (GPIO 24) auf Input setzen
    GPIO.setup(18, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
    raspberry=True

parser = argparse.ArgumentParser(description="Verzeichnis überwachen und neues Bild drucken.")
parser.add_argument('-d', '--dir', default=r"//192.168.179.25/pi/camera_ready", type=str, help='Verzeichnis mit den fertigen Bildern')
parser.add_argument('-ud', '--uploaddir', default=r"//192.168.179.25/pi/camera", type=str, help='Verzeichnis mit den Bildern beim Upload')
parser.add_argument('-pd', '--printdir', default=r"//192.168.179.25/pi/camera_print", type=str, help='Verzeichnis mit den gedruckten Bildern')
parser.add_argument('-td', '--trashdir', default=r"//192.168.179.25/pi/camera_trash", type=str, help='Verzeichnis mit den gelöschten Bildern')
parser.add_argument('-sw', '--screenwidth', default=1366, type=int, help='X-Auflösung des Bildschirms')
parser.add_argument('-sh', '--screenheight', default=768, type=int, help='Y-Auflösung des Bildschirms')
parser.add_argument('-pw', '--printwidth', default=900, type=int, help='X-Auflösung des Druckers')
parser.add_argument('-ph', '--printheight', default=600, type=int, help='Y-Auflösung des Druckers')
parser.add_argument('-ssw', '--slideshow_wait', default=60, type=int, help='Wartezeit bis zum Beginn der Slideshow')
parser.add_argument('-ssi', '--slideshow_interval', default=15, type=int, help='Intervallzeit der Slideshow')
args = parser.parse_args()

dir = args.dir
uploaddir = args.uploaddir
printdir = args.printdir
trashdir = args.trashdir
screensize = (args.screenwidth,args.screenheight)
printsize = (args.printwidth,args.printheight)
slideshow_wait = args.slideshow_wait
slideshow_interval = args.slideshow_interval

mouse_x = 0
mouse_y = 0
mouse_click = False
def mouseevent(event,x,y,flags,param):
    mouse_x = x
    mouse_y = y
    if event == cv2.EVENT_LBUTTONDOWN:
        mouse_click = True

# ein Fenster im Vollbild darstellen
cv2.namedWindow('screen',cv2.WINDOW_NORMAL)
cv2.setWindowProperty('screen',cv2.WND_PROP_FULLSCREEN,cv2.cv.CV_WINDOW_FULLSCREEN)
cv2.setMouseCallback('screen',mouseevent)

def getminmaxfilenum(dir):
    # liefert die kleinste und höchste Dateinummer in einem Verzeichnis
    minnum=10000
    maxnum=-1
    files = os.listdir(dir)
    for file in files:
        match = re.match(r'IMG_(\d+).JPG',file)
        if match:
            num = int(match.group(1))
            if num>maxnum:
                maxnum = num
            if num<minnum:
                minnum = num
    return minnum, maxnum

def getimgfilename(filenum):
    return "IMG_{0:04d}.JPG".format(filenum)

def getnextimgfilenum(dir,filenum,direction):
    while filenum>0 and filenum<10000:
        if os.path.isfile(dir+'/'+getimgfilename(filenum)):
            break
        filenum += direction
    return filenum

def rotate_90n(src, angle):
    dst = None
    if angle == 270 or angle == -90:
        #Rotate clockwise 270 degrees
        dst = cv2.transpose(src)
        dst = cv2.flip(dst, 0)
    elif angle == 180 or angle == -180:
        # Rotate clockwise 180 degrees
        dst = cv2.flip(src, -1)
    elif angle == 90 or angle == -270:
        # Rotate clockwise 90 degrees
        dst = cv2.transpose(src)
        dst = cv2.flip(dst, 1)
    elif angle == 360 or angle == 0 or angle == -360:
        dst = src
    else:
        raise
    return dst

minfilenum = 10000
lastfilenum = 0
lastfilenum_mem = 0
filenum = 0
processedfilenum = 0
key=0
do_print = False
drucktaste_flanke = False
img = None
imgfilename = None
imgrotate=0
end = False
lastaction_time = time.time()
slidelist = []
lastslide_time = lastaction_time;
while not end:
    try:
        ikey=0
        # Wird gerade etwas hochgeladen? Dann sind Dateien im upload-Verzeichnis
        umin, uploadnum = getminmaxfilenum(uploaddir)

        # Verzeichnis durchsuchen und Datei mit der höchsten Nummer ermitteln
        minfilenum, lastfilenum = getminmaxfilenum(dir)

        if lastfilenum != lastfilenum_mem:
            # Wenn sich letzte Datei geändert hat, dann auf letzte Datei setzen
            lastfilenum_mem = lastfilenum
            filenum = lastfilenum
            lastaction_time = time.time()

        if filenum != processedfilenum:
            processedfilenum = filenum
            imgfilename = getimgfilename(filenum)
            f = dir+'/'+imgfilename
            if os.path.isfile(f):
                pilimg = Image.open(f)
                try:
                    for orientation in ExifTags.TAGS.keys():
                        if ExifTags.TAGS[orientation]=='Orientation':
                            break
                    exif=dict(pilimg._getexif().items())
                finally:
                    del pilimg
                imgrotate=0
                if exif[orientation] == 3:
                    imgrotate = 180
                elif exif[orientation] == 6:
                    imgrotate = 90
                elif exif[orientation] == 8:
                    imgrotate = 270
                img = cv2.imread(f)

        img_disp = np.zeros((screensize[1],screensize[0],3), np.uint8)
        if img is not None:
            img_rot = rotate_90n(img,imgrotate)
            h,w = img_rot.shape[:2]
            screenratio = screensize[0]/float(screensize[1])
            pictureratio = w/float(h)
            if screenratio>pictureratio:
                disp_h = screensize[1]
                disp_w = int(disp_h*pictureratio)
                disp_y = 0
                disp_x = (screensize[0]-disp_w)/2
            else:
                disp_w = screensize[0]
                disp_h = int(disp_w/pictureratio)
                disp_x = 0
                disp_y = (screensize[1]-disp_h)/2
            img_resize = cv2.resize(img_rot, (disp_w,disp_h), 0, 0, cv2.cv.CV_INTER_AREA)
            # kleineres Bild auf den screen kopieren
            img_disp[disp_y:disp_y+disp_h, disp_x:disp_x+disp_w] = img_resize
        if raspberry:
            drucktaste = (GPIO.input(18) == GPIO.HIGH)
            do_print = drucktaste and not drucktaste_flanke
            drucktaste_flanke = drucktaste
        if not do_print:
            do_print = key == 'd'
            if do_print:
                key = 0
        if img is None:
            # Notbremse, falls hier doch kein Bild vorliegt
            do_print = False
        if do_print:
            # Bild soll gedruckt werden
            do_print = False
            # Anzeige erzeugen
            cv2.putText(img_disp,"Bild wird gedruckt...",(0,50),cv2.FONT_ITALIC, 2, (200,255,155), 2)
            cv2.imshow('screen',img_disp)
            k = cv2.waitKey(1)
            if ikey==0:
                ikey = k
            img_print = cv2.resize(img, printsize, 0, 0, cv2.cv.CV_INTER_AREA)
            printfile = printdir+'/'+imgfilename
            cv2.imwrite(printfile,img_print)
            if raspberry:
                call(['obexftp', '-b', '00:04:48:17:92:E7', '--nopath', '--noconn', '--uuid', 'none', '--channel', '1', '-p', printfile])
            lastaction_time = time.time()
            time.sleep(1)
        elif uploadnum>0:
            # Ladeanzeige, nur wenn nicht gerade gedruckt wird
            cv2.putText(img_disp,"Bild "+str(uploadnum)+" wird geladen...",(0,50),cv2.FONT_ITALIC, 2, (200,255,155), 2)
            lastaction_time = time.time()

        if time.time()-lastaction_time > slideshow_wait:
            if time.time()-lastslide_time > slideshow_interval:
                lastslide_time = time.time()
                if len(slidelist)==0:
                    slidelist = range(minfilenum,lastfilenum)
                i = random.randint(0,len(slidelist)-1)
                num = slidelist[i]
                del(slidelist[i])
                filenum = getnextimgfilenum(dir,num,1)
                if num != filenum:
                    # Die Dateien zwischen num und filenum existieren nicht
                    # Also diese noch aus der Liste entfernen
                    i = 0
                    while i < len(slidelist):
                        if slidelist[i]>num and slidelist[i]<=filenum:
                            del(slidelist[i])
                        else:
                            i += 1

        else:
            lastslide_time = lastaction_time - slideshow_interval;

        cv2.imshow('screen',img_disp)
        if ikey<=0:
            ikey = cv2.waitKey(10)
        while ikey>0:
            lastaction_time = time.time()
            key = chr(ikey & 255)
            if ikey>0:
                print "Taste:",ikey,",",ikey & 255
            if (ikey & 255) == 27:
                end = True
                break
            elif key == 'l':
                # aktuelle Datei Löschen
                os.rename(dir+'/'+imgfilename,trashdir+'/'+imgfilename)
                if not os.path.isfile(dir+'/'+imgfilename):
                    cv2.putText(img_disp,"Bild "+imgfilename+" wurde geloescht.",(0,50),cv2.FONT_ITALIC, 2, (200,255,155), 2)
                    filenum = getnextimgfilenum(dir,filenum,-1)
                    if filenum<minfilenum:
                        filenum = getnextimgfilenum(dir,minfilenum,1)
                else:
                    cv2.putText(img_disp,"Bild "+imgfilename+" konnte nicht geloescht werden.",(0,50),cv2.FONT_ITALIC, 2, (200,255,155), 2)
                cv2.imshow('screen',img_disp)
                time.sleep(1)
            # Sondertasten auswerten (die großen Zahlen sind Windows die kleinen Linux)
            elif key == 's':
                # Slideshow starten
                lastaction_time = time.time() - slideshow_wait
            elif ikey == 2490368: # up
                pass
            elif ikey == 2621440: # down
                pass
            elif ikey == 2424832 or ikey == 65361: #left
                if filenum>0:
                    filenum = getnextimgfilenum(dir,filenum-1,-1)
            elif ikey == 2555904 or ikey == 65363: #right
                if filenum<lastfilenum:
                    filenum = getnextimgfilenum(dir,filenum+1,1)
            elif ikey == 2293760 or ikey == 65367: # End
                filenum = lastfilenum
            elif ikey == 2359296 or ikey == 65360: # Pos1
                filenum = getnextimgfilenum(dir,minfilenum,1)
            ikey = cv2.waitKey(1)
    except:
        pass
cv2.destroyAllWindows()