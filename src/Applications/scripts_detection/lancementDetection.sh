#!/bin/bash
systemctl stop serial-getty@ttyS0
python /home/pi/Applications/scripts_detection/scriptSystemeDetection.py  &
python /home/pi/Applications/scripts_detection/scriptLogDetection.py 
