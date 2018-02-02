/* sound-pressure-level-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * pcm.h: Functions to calculate spectrum/spl/dba from pcm
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef PCM_H
#define PCM_H

#include <stdbool.h>
#include <stdint.h>

#include <arm_math.h>
#include "arm_const_structs.h"

#define PCM_BUFFER_SIZE 1024
#define PCM_FFT_SIZE (PCM_BUFFER_SIZE/2)

typedef enum {
	PCM_WEIGHTING_A = 0,
	PCM_WEIGHTING_B,
	PCM_WEIGHTING_C,
	PCM_WEIGHTING_D,
	PCM_WEIGHTING_Z,
	PCM_WEIGHTING_ITU_R_468
} PCMWeighting;

typedef enum {
	PCM_STATE_HAMMING_WINDOW,
	PCM_STATE_FFT,
	PCM_STATE_NORMALIZATION,
	PCM_STATE_EQUALIZATION,
	PCM_STATE_WEIGHTING,
	PCM_STATE_UPDATE_API
} PCMState;

typedef struct {
	// The very first byte of the PCM stream is a dummy byte. The first byte may be broken
	// and we don't want to add lots of special case handling in the interrupt handler.

	// Additionally we may overwrite up to 16 bytes at the end of the buffer, since we
	// only check for end of buffer every 16 bytes.
	int32_t buffer[PCM_BUFFER_SIZE+1 + 16];
	int32_t fft[PCM_BUFFER_SIZE]; // The arm fft algorithm needs the pcm buffer size *2 internally for the fft calculation
	arm_rfft_instance_q31 fft_instance;
	PCMWeighting weighting;
	PCMWeighting weighting_new;
	PCMState state;

	bool buffer_ready;

	uint16_t api_decibel;
	uint16_t api_spectrum[PCM_FFT_SIZE];
	uint16_t api_spectrum_tmp[PCM_FFT_SIZE];
	bool     api_spectrum_tmp_filled;
	bool     api_spectrum_new_available;
	uint16_t api_spectrum_index;
	uint16_t api_spectrum_cb_index;
	uint16_t api_spectrum_period;

	uint8_t  api_fft_size;
	bool     api_fft_size_new;
} PCM;

void pcm_init(void);
void pcm_tick(void);
uint16_t pcm_get_decibel(void);

#endif
