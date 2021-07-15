#!/usr/bin/env python
# -*- coding: utf-8 -*-
# lsusb to check device name
#dmesg | grep "tty" to find port name

import serial, time
import RPi.GPIO as GPIO
from datetime import date, datetime, timedelta


if __name__ == '__main__':

    GPIO.setmode(GPIO.BCM)
    GPIO.setup(20, GPIO.IN)


    print('Running. Press CTRL-C to exit.')
    with serial.Serial("/dev/ttyS0", 9600, timeout=1) as arduino:
        time.sleep(1) #wait for serial to open
        if arduino.isOpen():
            print("{} connected!".format(arduino.port))
            try:
                while True:
                    if not GPIO.input(20):
                        arduino.write("1")
                        time.sleep(0.01)

                   # while arduino.inWaiting()==0: pass
                   # if  arduino.inWaiting()>0: 
                   #     print("on recoit un truc du arduino")
                   #     answer=arduino.readline()
                   #     if(answer=="Son" or answer=="Presence"):
                   #         totalRoutineTime=arduino.readline()
                   #         routineTime=arduino.readline()
                   #         now=datetime.now()
                   #         startingTime=now.strftime("%s")-totalRoutineTime
                   #         duration=totalRoutineTime-routineTime

                   #         print("detection " + answer + " : " + str(now.strftime("%d-%m-%Y,%H:%M:%S, duration="))+str(duration))
                        arduino.flushInput() #remove data after reading

            except KeyboardInterrupt:
                print("KeyboardInterrupt has been caught.")
