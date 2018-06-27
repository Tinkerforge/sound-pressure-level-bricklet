#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletSoundPressureLevel;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your Sound Pressure Level Bricklet

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $spl = Tinkerforge::BrickletSoundPressureLevel->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Get current decibel
my $decibel = $spl->get_decibel();
print "Decibel: " . $decibel/10.0 . " dB(A)\n";

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
