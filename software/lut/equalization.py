#!/usr/bin/env python
# -*- coding: utf-8 -*-

import math
import numpy
import matplotlib.pyplot as plt

SIZE = 1024
MUL = 0xFFFF 
FREQ_FS = 40960.0

# These values are estimated from the graph from the
# ICS43432 datasheet. This will definitely increase the accuracy 
# in the 8kHz to 20kHz range, but it is not a real calibration
# you would be able to do in an anechoic chamber.
# However: There is an additional ~5dB variation between different
# ICS43432s that makes it impracitcal to go to an anechoic chamber
# for calibration of the high frequency values. The calibration would
# only be valid for the one tested microphone and it is not practical
# the calibrate each Bricklet.
x = numpy.array([1000, 6000, 7000, 8000, 9000, 10000, 20000])
y = numpy.array([0, 1.1214953271028, 1.58878504672897, 1.86915887850467, 2.05607476635514, 2.52336448598131, 14.0186915887851])

p = numpy.poly1d(numpy.polyfit(x, y, 3))

s = 'const uint16_t lut_equalization[{0}] = {{'.format(SIZE/2)

delta = FREQ_FS/SIZE
values = []
values_plot = []
for i in range(SIZE/2):
    f = i*delta + delta # Use mid of each frequency
    if f < 1000:
        value = 1
    else:
        value = 1/(10**(p(f)/20))
        if value > 1:
            value = 1
        values_plot.append(p(f))
    values.append(str(int(round((value*MUL)))))

s += ', '.join(values) + '};';
print(s)

plt.plot(values_plot)
plt.xscale("log")
plt.show()
