#!/usr/bin/env python
# -*- coding: utf-8 -*-

import math

SIZE = 1024
MUL = 0xFFFF
FREQ_FS = 40960.0

# https://en.wikipedia.org/wiki/ITU-R_468_noise_weighting
def h1(f):
    return -4.737338981378384*(10**-24)*(f**6) + 2.043828333606125*(10**-15)*(f**4) - 1.363894795463638*(10**-7)*(f**2) + 1

def h2(f):
    return 1.306612257412824*(10**-19)*(f**5) - 2.118150887518656*(10**-11)*(f**3) + 5.559488023498642*(10**-4)*f

def ITU_R_468(f):
    return 1.246332637532143*(10**-4)*f/math.sqrt(h1(f)**2 + h2(f)**2)

s = 'const uint16_t lut_itu_r_468_weighting[{0}] = {{'.format(SIZE/2)

delta = FREQ_FS/SIZE
values = []
for i in range(SIZE/2):
    f = i*delta + delta # Use mid of each frequency 
    value = ITU_R_468(f)
    values.append(str(int(round((value*MUL)))))

s += ', '.join(values) + '};';
print(s)
