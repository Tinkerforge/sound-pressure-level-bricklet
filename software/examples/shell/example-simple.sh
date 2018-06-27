#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Sound Pressure Level Bricklet

# Get current decibel
tinkerforge call sound-pressure-level-bricklet $uid get-decibel
