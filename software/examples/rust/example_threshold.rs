use std::{error::Error, io, thread};
use tinkerforge::{ipconnection::IpConnection, sound_pressure_level_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Sound Pressure Level Bricklet

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let sound_pressure_level_bricklet = SoundPressureLevelBricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    //Create listener for decibel events.
    let decibel_listener = sound_pressure_level_bricklet.get_decibel_receiver();
    // Spawn thread to handle received events. This thread ends when the sound_pressure_level_bricklet
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for event in decibel_listener {
            println!("Decibel: {}{}", event as f32 / 10.0, " dB(A)");
        }
    });

    // Configure threshold for decibel "greater than 60 dB(A)"
    // with a debounce period of 1s (1000ms)
    sound_pressure_level_bricklet.set_decibel_callback_configuration(1000, false, '>', 60 * 10, 0);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
