function matlab_example_threshold()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletSoundPressureLevel;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your Sound Pressure Level Bricklet

    ipcon = IPConnection(); % Create IP connection
    spl = handle(BrickletSoundPressureLevel(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register decibel callback to function cb_decibel
    set(spl, 'DecibelCallback', @(h, e) cb_decibel(e));

    % Configure threshold for decibel "greater than 60 dB(A)"
    % with a debounce period of 1s (1000ms)
    spl.setDecibelCallbackConfiguration(1000, false, '>', 60*10, 0);

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for decibel callback
function cb_decibel(e)
    fprintf('Decibel: %g dB(A)\n', e.decibel/10.0);
end
