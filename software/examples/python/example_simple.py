#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your Sound Pressure Level Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_sound_pressure_level import BrickletSoundPressureLevel

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    spl = BrickletSoundPressureLevel(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Get current decibel
    decibel = spl.get_decibel()
    print("Decibel: " + str(decibel/10.0) + " dB(A)")

    input("Press key to exit\n") # Use raw_input() in Python 2
    ipcon.disconnect()
