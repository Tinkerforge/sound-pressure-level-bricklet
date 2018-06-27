function octave_example_simple()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Sound Pressure Level Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    spl = javaObject("com.tinkerforge.BrickletSoundPressureLevel", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current decibel
    decibel = spl.getDecibel();
    fprintf("Decibel: %g dB(A)\n", decibel/10.0);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end
