import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletSoundPressureLevel;

public class ExampleCallback {
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

		// Set period for decibel callback to 1s (1000ms) without a threshold
		spl.setDecibelCallbackConfiguration(1000, false, 'x', 0, 0);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
