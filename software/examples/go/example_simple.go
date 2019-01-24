package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
	"github.com/Tinkerforge/go-api-bindings/sound_pressure_level_bricklet"
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

	// Get current decibel.
	decibel, _ := spl.GetDecibel()
	fmt.Printf("Decibel: %f dB(A)\n", float64(decibel)/10.0)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()
}
