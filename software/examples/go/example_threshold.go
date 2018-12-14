package main

import (
	"fmt"
	"tinkerforge/ipconnection"
	"tinkerforge/sound_pressure_level_bricklet"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your Sound Pressure Level Bricklet.

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	spl, _ := sound_pressure_level_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	spl.RegisterDecibelCallback(func(decibel uint16) {
		fmt.Printf("Decibel: %d dB(A)\n", float64(decibel)/10.0)
	})

	// Configure threshold for decibel "greater than 60 dB(A)"
	// with a debounce period of 1s (1000ms).
	spl.SetDecibelCallbackConfiguration(1000, false, '>', 60*10, 0)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

}
