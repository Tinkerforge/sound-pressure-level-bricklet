#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_sound_pressure_level'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your Sound Pressure Level Bricklet

ipcon = IPConnection.new # Create IP connection
spl = BrickletSoundPressureLevel.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Get current decibel
decibel = spl.get_decibel
puts "Decibel: #{decibel/10.0} dB(A)"

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
