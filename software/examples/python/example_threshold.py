#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your Sound Pressure Level Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_sound_pressure_level import BrickletSoundPressureLevel

# Callback function for decibel callback
def cb_decibel(decibel):
    print("Decibel: " + str(decibel/10.0) + " dB(A)")

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    spl = BrickletSoundPressureLevel(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Register decibel callback to function cb_decibel
    spl.register_callback(spl.CALLBACK_DECIBEL, cb_decibel)

    # Configure threshold for decibel "greater than 60 dB(A)"
    # with a debounce period of 1s (1000ms)
    spl.set_decibel_callback_configuration(1000, False, ">", 60*10, 0)

    input("Press key to exit\n") # Use raw_input() in Python 2
    ipcon.disconnect()
