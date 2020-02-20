#!/usr/bin/env python3

import requests
from datetime import datetime

def main():
    with open("data.txt") as f:
        for line in f:
            l = line.split("AoA is at: ")
            if len(l) == 2:
                # Send AoA
                aoa = l[1]
                # Post AoA
                # Hardcode AP
                ap = 1
                timestamp = datetime.now().isoformat()
                packet = {
                    "access_point": ap,
                    "timestamp": timestamp,
                    "angle_of_arrival": "aoa",
                }
                print(packet)

if __name__ == "__main__":
    main()
    