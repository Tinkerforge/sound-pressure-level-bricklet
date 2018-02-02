#!/usr/bin/env python
# -*- coding: utf-8 -*-

import math

SIZE = 1024
MUL = 0xFFFF
FREQ_FS = 40960.0

# https://en.wikipedia.org/wiki/A-weighting#B
def R_B(f):
    return ((12194**2)*(f**3))/(((f**2) + (20.6**2)) * math.sqrt((f**2) + (158.5**2)) * ((f**2) + (12194**2)))

s = 'const uint16_t lut_b_weighting[{0}] = {{'.format(SIZE/2)

delta = FREQ_FS/SIZE
values = []
for i in range(SIZE/2):
    f = i*delta + delta # Use mid of each frequency 
    value = R_B(f)
    values.append(str(int(round((value*MUL)))))

s += ', '.join(values) + '};';
print(s)
