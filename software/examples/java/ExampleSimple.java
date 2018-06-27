import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletSoundPressureLevel;

public class ExampleSimple {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your Sound Pressure Level Bricklet
	private static final String UID = "XYZ";

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletSoundPressureLevel spl =
		  new BrickletSoundPressureLevel(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Get current decibel
		int decibel = spl.getDecibel(); // Can throw com.tinkerforge.TimeoutException
		System.out.println("Decibel: " + decibel/10.0 + " dB(A)");

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
