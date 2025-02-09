import serial
import time
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
ser.flush()
while (1):
  send_string = ("342")
  ser.write(send_string.encode('utf-8'))
  time.sleep(2)
  receive_string = ser.readline().decode('utf-8').rstrip()
  print(receive_string)
