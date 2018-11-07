use std::{error::Error, io, thread};
use tinkerforge::{ip_connection::IpConnection, sound_pressure_level_bricklet::*};

const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Sound Pressure Level Bricklet.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let spl = SoundPressureLevelBricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
                                          // Don't use device before ipcon is connected.

    // Create receiver for decibel events.
    let decibel_receiver = spl.get_decibel_receiver();

    // Spawn thread to handle received events. This thread ends when the `spl` object
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for decibel in decibel_receiver {
            println!("Decibel: {} dB(A)", decibel as f32 / 10.0);
        }
    });

    // Configure threshold for decibel "greater than 60 dB(A)"
    // with a debounce period of 1s (1000ms).
    spl.set_decibel_callback_configuration(1000, false, '>', 60 * 10, 0);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
