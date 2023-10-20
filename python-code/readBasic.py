import datetime as dt
import msvcrt
import os
import time

import pandas as pd
import serial


def create_csv(columnsList=None):
    # get current date and time
    current_time = dt.datetime.now().strftime("%m-%d-%y_%I-%M-%p")

    #  creates a new csv
    filename = input("Enter new filename: ")
    filename_path = f"{os.path.dirname(__file__)}\{filename}_{current_time}.csv"
    pd.DataFrame(columns=columnsList).to_csv(filename_path, index=False)

    return filename_path


def connect_and_collect(path):
    time.sleep(3)
    serial_com = serial.Serial("COM4", 115200)  # connect to arduino

    # Reset the arduino
    serial_com.setDTR(False)
    time.sleep(1)
    serial_com.flushInput()
    serial_com.setDTR(True)

    values = []

    # starts collecting data
    time_on = True
    while time_on:
        while serial_com.inWaiting() == 0:
            pass
        # read a line of data
        data_bytes = serial_com.readline()
        decoded_bytes = data_bytes.decode("utf-8").strip("\r\n")
        print(decoded_bytes)
        try:
            values = [float(x) for x in decoded_bytes.split(",")]
        except ValueError:
            print("Could not convert string to float.")

        if len(values) == 5:
            data = {
                "time": [values[0]],
                "true_phi": [values[1]],
                "phi_hat": [values[2]],
                "phi_hat2": [values[3]],
                "phi_hat3": [values[4]],
            }
            # make dataframe from data
            df = pd.DataFrame(data)
            df.to_csv(path, mode="a", header=False, index=False)

        # if 10 secs has past (Collect data for 10 secs)
        if values[0] == 10:
            time_on = False

        # exit while loop with ENTER key input
        # if msvcrt.kbhit():
        #     if msvcrt.getwche() == "\r":
        #         # time_on = False
        #         break
    print("Done")
