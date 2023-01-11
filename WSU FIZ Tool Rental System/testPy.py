from arduino import *
##from sys import exit
scanner_port = openSerialPort("COM5")
print(scanner_port)
test = input("test")
writeSerial(scanner_port, "b")
test = input("test")
while True:
    localID = readSerial(scanner_port)
    if localID is not None:
        print(localID)
