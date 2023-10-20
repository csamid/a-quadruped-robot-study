from serial.tools import list_ports
import serial
import time
import csv

ports = list_ports.comports()
for port in ports:
    print(port)

f = open("data.csv", "w", newline="")  # create data file
f.truncate()  # clears any existing data

serialCom = serial.Serial("COM4", 115200)  # connect to arduino

# Reset the arduino
serialCom.setDTR(False)
time.sleep(1)
serialCom.flushInput()
serialCom.setDTR(True)

kmax = 10000  # num of data points to read
for k in range(kmax):
    ##try:
    # Read a line of data
    s_bytes = serialCom.readline()
    # Decode binary
    decoded_bytes = s_bytes.decode("utf-8").strip("\r\n")
    # print(decoded_bytes)

    # Parse lines
    if k == 0:
        values = decoded_bytes.split(",")
    else:
        values = [float(x) for x in decoded_bytes.split()]
    print(values)

    # write data to csv use writerow function
    writer = csv.writer(f, delimiter=",")
    writer.writerow(values)

    ##except:
    ##print("ERROR. Line was not recorded")

f.close()  # close csv file
