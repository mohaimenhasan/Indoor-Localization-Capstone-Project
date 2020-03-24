#!/usr/bin/env python3

import requests
from datetime import datetime

def process_frame(frame):
    return [data.replace('[', '').replace(']', '') for data in frame]

def main():
    with open("1.txt") as f:
        lines = f.readlines()
        for i in range(0, len(lines)):
            line = lines[i]
            if line.startswith("CSI FRAME"):
                # csi_frame = lines[i+1]
                temp = i
                i += 1
                line = lines[i]
                csi_frame = []
                while(not line.startswith("start calculation")):
                    csi_frame.append(line.strip())
                    i += 1
                    line = lines[i]
                csi_frame = process_frame(csi_frame)
                i = temp
                line = lines[i]
            if line.startswith("GRID"):
                temp = i
                i += 1
                line = lines[i]
                grid = []
                while(not line.startswith("AoA")):
                    line = line.replace('[', '').replace(']', '')
                    grid += line.split()
                    i += 1
                    line = lines[i]
                i = temp
                line = lines[i]
            if line.startswith("AoA is at: "):
                l = line.split("AoA is at: ")[1]
                # Send AoA
                aoa = l.split()[0]
                # Post AoA
                # Hardcode AP
                ap = 1
                timestamp = datetime.now().isoformat()
                # To send to backend 
                url = "http://localhost:8888/receiver/addData"
                packet = {
                    "access_point": ap,
                    "timestamp": timestamp,
                    "angle_of_arrival": aoa,
                    "csi_frame": csi_frame,
                    "grid":grid,
                }
                print("Sending following packet: ")
                print(packet)
                # print("Data: " + str(packet))
                # Uncomment to send POST request
                r = requests.post(url = url, json = packet)
                print("Reqest response: " + str(r))
                print("-----")

    with open("2.txt") as f:
        lines = f.readlines()
        for i in range(0, len(lines)):
            line = lines[i]
            if line.startswith("CSI FRAME"):
                # csi_frame = lines[i+1]
                temp = i
                i += 1
                line = lines[i]
                csi_frame = []
                while(not line.startswith("start calculation")):
                    csi_frame.append(line.strip())
                    i += 1
                    line = lines[i]
                csi_frame = process_frame(csi_frame)
                i = temp
                line = lines[i]
            if line.startswith("GRID"):
                temp = i
                i += 1
                line = lines[i]
                grid = []
                while(not line.startswith("AoA")):
                    line = line.replace('[', '').replace(']', '')
                    grid += line.split()
                    i += 1
                    line = lines[i]
                i = temp
                line = lines[i]
            if line.startswith("AoA is at: "):
                l = line.split("AoA is at: ")[1]
                # Send AoA
                aoa = l.split()[0]
                # Post AoA
                # Hardcode AP
                ap = 2
                timestamp = datetime.now().isoformat()
                # To send to backend
                url = "http://localhost:8888/receiver/addData"
                packet = {
                    "access_point": ap,
                    "timestamp": timestamp,
                    "angle_of_arrival": aoa,
                    "csi_frame": csi_frame,
                    "grid":grid,
                }
                print("Sending following packet: ")
                # print("Data: " + str(packet))
                # Uncomment to send POST request
                r = requests.post(url = url, json = packet)
                print(packet)
                print("Reqest response: " + str(r))
                print("-----")

    with open("3.txt") as f:
        lines = f.readlines()
        for i in range(0, len(lines)):
            line = lines[i]
            if line.startswith("CSI FRAME"):
                # csi_frame = lines[i+1]
                temp = i
                i += 1
                line = lines[i]
                csi_frame = []
                while(not line.startswith("start calculation")):
                    csi_frame.append(line.strip())
                    i += 1
                    line = lines[i]
                csi_frame = process_frame(csi_frame)
                i = temp
                line = lines[i]
            if line.startswith("GRID"):
                temp = i
                i += 1
                line = lines[i]
                grid = []
                while(not line.startswith("AoA")):
                    line = line.replace('[', '').replace(']', '')
                    grid += line.split()
                    i += 1
                    line = lines[i]
                i = temp
                line = lines[i]
            if line.startswith("AoA is at: "):
                l = line.split("AoA is at: ")[1]
                # Send AoA
                aoa = l.split()[0]
                # Post AoA
                # Hardcode AP
                ap = 3
                timestamp = datetime.now().isoformat()
                # To send to backend
                url = "http://localhost:8888/receiver/addData"
                packet = {
                    "access_point": ap,
                    "timestamp": timestamp,
                    "angle_of_arrival": aoa,
                    "csi_frame": csi_frame,
                    "grid":grid,
                }
                print("Sending following packet: ")
                # print("Data: " + str(packet))
                # Uncomment to send POST request
                r = requests.post(url = url, json = packet)
                print(packet)
                print("Reqest response: " + str(r))
                print("-----")

if __name__ == "__main__":
    main()
    