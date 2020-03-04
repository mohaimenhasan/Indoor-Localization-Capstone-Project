#!/usr/bin/env python3

import requests
from datetime import datetime

def main():
    with open("temp.txt") as f:
        for line in f:
            if line.startswith("AoA is at: "):
                l = line.split("AoA is at: ")[1]
                # Send AoA
                aoa = l.split()[0]
                # Post AoA
                # Hardcode AP
                ap = 1
                timestamp = datetime.now().isoformat()
                # To send to backend 
                url = "http://172.20.10.2:8888/receiver/addData"
                packet = {
                    "access_point": ap,
                    "timestamp": timestamp,
                    "angle_of_arrival": aoa,
                }
                print("Sending post request ... ")
                print("Data: " + str(packet))
                r = requests.post(url = url, json = packet)
                print("Reqest response: " + str(r))
                print("-----")
                # print(json_packet)

if __name__ == "__main__":
    main()
    