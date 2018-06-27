using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your Sound Pressure Level Bricklet

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletSoundPressureLevel spl =
		  new BrickletSoundPressureLevel(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Get current decibel
		int decibel = spl.GetDecibel();
		Console.WriteLine("Decibel: " + decibel/10.0 + " dB(A)");

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
