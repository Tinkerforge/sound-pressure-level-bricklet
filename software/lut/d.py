#!/usr/bin/env python
# -*- coding: utf-8 -*-

import math

SIZE = 1024
MUL = 0xFFFF
FREQ_FS = 40960.0

# https://en.wikipedia.org/wiki/A-weighting#D
def h(f):
    return ((1037918.48 - (f**2))**2 + 1080768.16*(f**2))/((9837328 - (f**2))**2 + 11723776 * (f**2))

def R_D(f):
    return (f/(6.8966888496476*(10**-5)))*math.sqrt(h(f)/(((f**2) + 79919.29) * (f**2 + 1345600)))

s = 'const uint32_t lut_d_weighting[{0}] = {{'.format(SIZE/2)

delta = FREQ_FS/SIZE
values = []
for i in range(SIZE/2):
    f = i*delta + delta # Use mid of each frequency 
    value = R_D(f)
    values.append(str(int(round((value*MUL)))))

s += ', '.join(values) + '};';
print(s)
