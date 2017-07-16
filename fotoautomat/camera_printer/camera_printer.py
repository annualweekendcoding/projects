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
import sys

parser = argparse.ArgumentParser(description="Verzeichnis überwachen und neues Bild drucken.")
parser.add_argument('-d', '--dir', default="camera_ready", type=str, help='Verzeichnis mit den fertigen Bildern')
parser.add_argument('-ud', '--uploaddir', default="camera", type=str, help='Verzeichnis mit den Bildern beim Upload')
parser.add_argument('-pd', '--printdir', default="camera_print", type=str, help='Verzeichnis mit den gedruckten Bildern')
parser.add_argument('-td', '--trashdir', default="camera_trash", type=str, help='Verzeichnis mit den gelöschten Bildern')
parser.add_argument('-dp', '--dirprefix', default="", type=str, help='Prefix vor allen Verzeichnissen')
parser.add_argument('-sw', '--screenwidth', default=1366, type=int, help='X-Auflösung des Bildschirms')
parser.add_argument('-sh', '--screenheight', default=768, type=int, help='Y-Auflösung des Bildschirms')
parser.add_argument('-pw', '--printwidth', default=900, type=int, help='X-Auflösung des Druckers')
parser.add_argument('-ph', '--printheight', default=600, type=int, help='Y-Auflösung des Druckers')
parser.add_argument('-ssw', '--slideshow_wait', default=60, type=int, help='Wartezeit bis zum Beginn der Slideshow')
parser.add_argument('-ssi', '--slideshow_interval', default=15, type=int, help='Intervallzeit der Slideshow')
args = parser.parse_args()

dir = args.dirprefix + args.dir
uploaddir = args.dirprefix + args.uploaddir
printdir = args.dirprefix + args.printdir
trashdir = args.dirprefix + args.trashdir
screensize = (args.screenwidth,args.screenheight)
printsize = (args.printwidth,args.printheight)
slideshow_wait = args.slideshow_wait
slideshow_interval = args.slideshow_interval

# Die Dateinummer setzt sich aus 3 Stellen Verzeichnis und max 7 Stellen Datei zusammen
MAXFILENUM = 9999999999

# Unterscheidung wenn Linux wird hier der Raspberry angenommen, ansonsten wird ggf. unter Windows getestet.
raspberry=False
if platform.system()=='Linux':
    from subprocess import call
    import RPi.GPIO as GPIO
    # RPi.GPIO Layout verwenden (wie Pin-Nummern)
    GPIO.setmode(GPIO.BOARD)
    # Pin 18 (GPIO 24) auf Input setzen
    GPIO.setup(18, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
    # Flanke einlesen, falls Taste jetzt schon gedrueckt
    drucktaste_flanke = (GPIO.input(18) == GPIO.HIGH)
    raspberry=True

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

#globales Verzeichnis aller Zuordnungen zwischen Nummern und Dateinamen
filedic = {}

def getminmaxfilenum(dir):
    # liefert die kleinste und höchste Dateinummer in einem Verzeichnis
    minnum=MAXFILENUM+1
    maxnum=-1
    try:
        files = os.listdir(dir)
        for file in files:
            match = re.match(r'[A-Z_]*(\d+).JPG',file)
            if match:
                num = int(match.group(1))
                filedic[num] = file
                if num>maxnum:
                    maxnum = num
                if num<minnum:
                    minnum = num
    except:
        # Es ist egal was hier schief geht, dann haben wir nur keine Nummern
        pass
    return minnum, maxnum

def getnextimgfilenum(dir,filenum,direction):
    while filenum>0 and filenum<MAXFILENUM+1:
        if filenum in filedic:
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

def CenterPictureOnPicture(img,filename):
    # Diese Funktion setzt ein Bild aus einer Datei zentriert auf das übergebene Bild
    small = cv2.imread(filename)
    if not small is None:
        small_h,small_w = small.shape[:2]
        img_h,img_w = img.shape[:2]
        # TODO: Wenn Bild zu groß ist, dann verkleinern
        if small_w<img_w and small_h<img_h:
            y = (img_h-small_h)/2
            x = (img_w-small_w)/2
            img[y:y+small_h, x:x+small_w] = small
    return img

minfilenum = MAXFILENUM+1
lastfilenum = 0
lastfilenum_mem = 0
filenum = 0
processedfilenum = 0
key=0
do_print = False
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
            imgfilename = filedic[filenum]
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

            img_disp = CenterPictureOnPicture(img_disp,"Bild_wird_gedruckt.png")
            cv2.imshow('screen',img_disp)
            k = cv2.waitKey(1)
            if ikey==0:
                ikey = k
            #img_print = cv2.resize(img, printsize, 0, 0, cv2.cv.CV_INTER_AREA)
            #printfile = printdir+'/'+imgfilename
            #cv2.imwrite(printfile,img_print)
            if raspberry:
                # Bluetooth Drucker
                #call(['obexftp', '-b', '00:04:48:17:92:E7', '--nopath', '--noconn', '--uuid', 'none', '--channel', '1', '-p', printfile])
                # Druckjob auf Windows Rechner, Datei übertragen
                olddir = os.getcwd()
                # printdir, wenn oben resize erfolgt
                os.chdir(dir)
                call(['smbclient', '-A', '/home/pi/smbclient.conf', '//192.168.2.2/print', '-c', 'put '+imgfilename])
                os.chdir(olddir)
            lastaction_time = time.time()
            time.sleep(1)
        elif uploadnum>0:
            # Ladeanzeige, nur wenn nicht gerade gedruckt wird
            img_disp = CenterPictureOnPicture(img_disp,"Neue_Bilder_werden_geladen.png")
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
    except:
        print "Exception unbehandelt: ",sys.exc_info()[0]
        time.sleep(10)

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
            try:
                os.rename(dir+'/'+imgfilename,trashdir+'/'+imgfilename)
                img_disp = CenterPictureOnPicture(img_disp,"Bild_wurde_geloescht.png")
                filenum = getnextimgfilenum(dir,filenum,-1)
                if filenum<minfilenum:
                    filenum = getnextimgfilenum(dir,minfilenum,1)
            except:
                img_disp = CenterPictureOnPicture(img_disp,"Bild_konnte_nicht_geloescht_werden.png")
            cv2.imshow('screen',img_disp)
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
        elif key!='d':
            # Hilfe für die Tastenbelegung anzeigen
            img_help = cv2.imread("tastaturhilfe.png")
            cv2.imshow('screen',img_help)
            cv2.waitKey(10000)
        ikey = cv2.waitKey(1000)
cv2.destroyAllWindows()