import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletSoundPressureLevel;

public class ExampleThreshold {
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

		// Add decibel listener
		spl.addDecibelListener(new BrickletSoundPressureLevel.DecibelListener() {
			public void decibel(int decibel) {
				System.out.println("Decibel: " + decibel/10.0 + " dB(A)");
			}
		});

		// Configure threshold for decibel "greater than 60 dB(A)"
		// with a debounce period of 1s (1000ms)
		spl.setDecibelCallbackConfiguration(1000, false, '>', 60*10, 0);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
