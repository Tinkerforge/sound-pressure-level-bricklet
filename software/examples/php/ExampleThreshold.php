<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletSoundPressureLevel.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletSoundPressureLevel;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Sound Pressure Level Bricklet

// Callback function for decibel callback
function cb_decibel($decibel)
{
    echo "Decibel: " . $decibel/10.0 . " dB(A)\n";
}

$ipcon = new IPConnection(); // Create IP connection
$spl = new BrickletSoundPressureLevel(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Register decibel callback to function cb_decibel
$spl->registerCallback(BrickletSoundPressureLevel::CALLBACK_DECIBEL, 'cb_decibel');

// Configure threshold for decibel "greater than 60 dB(A)"
// with a debounce period of 1s (1000ms)
$spl->setDecibelCallbackConfiguration(1000, FALSE, '>', 60*10, 0);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
