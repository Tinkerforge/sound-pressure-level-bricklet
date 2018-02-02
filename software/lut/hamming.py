#!/usr/bin/env python
# -*- coding: utf-8 -*-

import math
from numpy import mean, sqrt, square, asarray

SIZE = 1024
MUL = 0xFFFF 

alpha = 25/46.0
beta = 21/46.0

s = 'const uint16_t lut_hamming[{0}] = {{'.format(SIZE)

values_hamming = []
values_rms = []
for n in range(SIZE):
    value = alpha - beta*math.cos(2*math.pi*n/(SIZE-1))
    values_hamming.append(str(int(round((value*MUL)))))
    values_rms.append(value)

s += ', '.join(map(str, values_hamming)) + '};';
s += '\nconst double hamming_rms = '
s += str(sqrt(mean(square(asarray(values_rms))))) + ';'
print(s)
