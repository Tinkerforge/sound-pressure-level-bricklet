<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletSoundPressureLevel.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletSoundPressureLevel;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Sound Pressure Level Bricklet

$ipcon = new IPConnection(); // Create IP connection
$spl = new BrickletSoundPressureLevel(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Get current decibel
$decibel = $spl->getDecibel();
echo "Decibel: " . $decibel/10.0 . " dB(A)\n";

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
