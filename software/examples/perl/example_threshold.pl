#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletSoundPressureLevel;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your Sound Pressure Level Bricklet

# Callback subroutine for decibel callback
sub cb_decibel
{
    my ($decibel) = @_;

    print "Decibel: " . $decibel/10.0 . " dB(A)\n";
}

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $spl = Tinkerforge::BrickletSoundPressureLevel->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Register decibel callback to subroutine cb_decibel
$spl->register_callback($spl->CALLBACK_DECIBEL, 'cb_decibel');

# Configure threshold for decibel "greater than 60 dB(A)"
# with a debounce period of 1s (1000ms)
$spl->set_decibel_callback_configuration(1000, 0, '>', 60*10, 0);

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
