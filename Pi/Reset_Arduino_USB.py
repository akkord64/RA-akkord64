import serial
import time
ser = serial.Serial("/dev/ttyUSB1", 115200, timeout=1)
ser.setDTR(False)
time.sleep(0.5)