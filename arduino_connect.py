
## Written by Mihir Joshi on 3/6/24

import time
from serial import Serial

def getArduinoSerial(port, baud):
    # specify arduino port and baud
    # port can be found via the Arduino IDE

    dataList = []
    # arduino_port = "/dev/tty.usbmodem2101"
    # Establish Serial object with COM port and BAUD rate to match Arduino Port/rate 
    ser = Serial(port, baud)  # ( i.e. port: /dev/tty.usbmodem2101, baud: 9600) NOTE: this must be the same as the Arduino IDE
    time.sleep(2)

    return dataList, ser   