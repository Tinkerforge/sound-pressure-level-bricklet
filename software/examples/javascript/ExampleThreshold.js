var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your Sound Pressure Level Bricklet

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var spl = new Tinkerforge.BrickletSoundPressureLevel(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Configure threshold for decibel "greater than 60 dB(A)"
        // with a debounce period of 1s (1000ms)
        spl.setDecibelCallbackConfiguration(1000, false, '>', 60*10, 0);
    }
);

// Register decibel callback
spl.on(Tinkerforge.BrickletSoundPressureLevel.CALLBACK_DECIBEL,
    // Callback function for decibel callback
    function (decibel) {
        console.log('Decibel: ' + decibel/10.0 + ' dB(A)');
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
