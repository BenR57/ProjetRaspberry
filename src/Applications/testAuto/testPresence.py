import serial, time
from datetime import datetime
import sys

duration=sys.argv[1]

duration=float(duration)
with serial.Serial("/dev/ttyS0", 9600, timeout=1) as arduino:
  time.sleep(1) 
  w="1"
  if arduino.isOpen():
    arduino.write(w.encode())
    for i in range(int(duration)):
      time.sleep(1)
      arduino.write(w.encode())
    time.sleep(duration-int(duration))
    arduino.write(w.encode())


   



