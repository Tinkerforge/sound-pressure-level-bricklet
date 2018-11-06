use std::{error::Error, io};

use tinkerforge::{ipconnection::IpConnection, sound_pressure_level_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Sound Pressure Level Bricklet

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let sound_pressure_level_bricklet = SoundPressureLevelBricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    // Get current decibel
    let decibel = sound_pressure_level_bricklet.get_decibel().recv()?;
    println!("Decibel: {}{}", decibel as f32 / 10.0, " dB(A)");

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
