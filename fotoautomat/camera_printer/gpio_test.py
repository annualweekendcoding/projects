#!/usr/bin/python2
# coding=UTF-8
import time

import RPi.GPIO as GPIO
# RPi.GPIO Layout verwenden (wie Pin-Nummern)
GPIO.setmode(GPIO.BOARD)
# Pin 18 (GPIO 24) auf Input setzen
GPIO.setup(18, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

while True:
	drucktaste = (GPIO.input(18) == GPIO.HIGH)
	print drucktaste
	print "\n"
	#time.sleep(1)

