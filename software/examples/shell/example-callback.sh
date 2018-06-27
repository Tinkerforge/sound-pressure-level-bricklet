#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Sound Pressure Level Bricklet

# Handle incoming decibel callbacks
tinkerforge dispatch sound-pressure-level-bricklet $uid decibel &

# Set period for decibel callback to 1s (1000ms) without a threshold
tinkerforge call sound-pressure-level-bricklet $uid set-decibel-callback-configuration 1000 false threshold-option-off 0 0

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
