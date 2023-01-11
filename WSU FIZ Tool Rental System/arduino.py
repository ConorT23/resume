import time
import serial
import serial.tools.list_ports

#Lists serial ports
#params: none
#returns: array
def listSerialPorts():
    serPorts = []
    for i in serial.tools.list_ports.comports():
        serPorts.append(i.device)
    return serPorts
    # print("Ports available: "+serial.tools.list_ports.comports().__len__().__str__())

#Opens serial port
#params: none
#returns: serial object
def openSerialPort(port):
    return serial.Serial(port, 9600)

#Reads from serial port
#params: serial port
#returns: none
def readSerial(port):
    if (port.inWaiting()>0):
        myData = port.readline()
        myData = str(myData)
        myData = myData[2:len(myData)-5]
        return myData

#Write to serial port
#params: serial port, data
#returns: none
def writeSerial(port, data):
    port.write(data.encode())
