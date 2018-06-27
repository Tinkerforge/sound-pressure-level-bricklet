function matlab_example_simple()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletSoundPressureLevel;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your Sound Pressure Level Bricklet

    ipcon = IPConnection(); % Create IP connection
    spl = handle(BrickletSoundPressureLevel(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current decibel
    decibel = spl.getDecibel();
    fprintf('Decibel: %g dB(A)\n', decibel/10.0);

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end
