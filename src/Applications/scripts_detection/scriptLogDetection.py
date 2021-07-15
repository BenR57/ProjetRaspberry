#!/usr/bin/env python
# -*- coding: utf-8 -*-
# lsusb to check device name
#dmesg | grep "tty" to find port name

import serial, time
from datetime import date, datetime, timedelta


if __name__ == '__main__':


    with serial.Serial("/dev/ttyS0", 9600, timeout=1) as arduino:
        time.sleep(1) #wait for serial to open
        if arduino.isOpen():
            try:
                while True:
                    while arduino.inWaiting()==0: pass
                    if  arduino.inWaiting()>0: 
                        answer=arduino.readline()                       
                        a=answer.split(":")
                        if(a[0]=="Son" or a[0]=="Presence"):
                            totalRoutineTime=int(a[1]) 
                            routineTime=int(a[2])          
                            now=datetime.now()
                            startingTime=int(now.strftime("%s"))-totalRoutineTime/1000.
                            duration=totalRoutineTime-routineTime

                            f=open("/home/pi/Applications/detectionEvents/ProjetRaspberry/detectionEvents.log", "a")
                            
                            #f.write(str(now.strftime("%Y-%m-%d %H:%M:%S")) + " detection " + a[0]+ " duration: "+ str(duration/1000.) + " sec\n")
                            dateTime=datetime.fromtimestamp(int(startingTime))
                            f.write(str(dateTime) + " detection " + a[0]+ " duration: "+ str(duration/1000.) + " sec\n" )
                            f.close()
                        arduino.flushInput() #remove data after reading

            except KeyboardInterrupt:
                print("KeyboardInterrupt has been caught.")
