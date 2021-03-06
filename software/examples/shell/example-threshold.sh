#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Sound Pressure Level Bricklet

# Handle incoming decibel callbacks
tinkerforge dispatch sound-pressure-level-bricklet $uid decibel &

# Configure threshold for decibel "greater than 60 dB(A)"
# with a debounce period of 1s (1000ms)
tinkerforge call sound-pressure-level-bricklet $uid set-decibel-callback-configuration 1000 false threshold-option-greater 600 0

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
