/* sound-pressure-level-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * pcm.c: Functions to calculate spectrum/spl/dba from pcm
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

#include "pcm.h"

#include <stdint.h>
#include <string.h>
#include <math.h>

#include <arm_math.h>
#include "arm_const_structs.h"
#include "bricklib2/hal/uartbb/uartbb.h"
#include "bricklib2/hal/system_timer/system_timer.h"
#include "bricklib2/utility/util_definitions.h"
#include "bricklib2/os/coop_task.h"

#include "communication.h"

#include "xmc_gpio.h"
#include "xmc_math.h"

#include "ics43432.h"


extern const double   lut_hamming_rms;
extern const uint16_t lut_hamming[];
extern const uint16_t lut_equalization[];
extern const uint16_t lut_a_weighting[];
extern const uint16_t lut_b_weighting[];
extern const uint16_t lut_c_weighting[];
extern const uint32_t lut_d_weighting[];
extern const uint16_t lut_itu_r_468_weighting[];

const uint16_t pcm_fft_sizes[4] = {128, 256, 512, 1024};
const uint16_t pcm_fft_multiplier[4] = {8, 4, 2, 1};

PCM pcm;
CoopTask pcm_task;

static void pcm_remove_dc_offset(void) {
	int64_t sum = 0;
	for(uint16_t i = 1; i < pcm_fft_sizes[pcm.api_fft_size]+1; i++) {
		sum += pcm.buffer[i];
	}

	int32_t avg = sum/pcm_fft_sizes[pcm.api_fft_size];

	for(uint16_t i = 1; i < pcm_fft_sizes[pcm.api_fft_size]+1; i++) {
		pcm.buffer[i] -= avg;
	}
}

static void pcm_apply_hamming_window(void) {
	for(uint16_t i = 1; i < pcm_fft_sizes[pcm.api_fft_size]+1; i++) {
		if((i % 128) == 0) {
			coop_task_yield();
		}
		// 12 bit for the lut and 8 bit because the pcm data is 24 bit but saved as 32 bit
		pcm.buffer[i] = ((int64_t)pcm.buffer[i])*lut_hamming[i*pcm_fft_multiplier[pcm.api_fft_size]] / (1 << (16+8));
	}
}

static void pcm_apply_fft(void) {
	arm_rfft_q31(&pcm.fft_instance, (q31_t*)(pcm.buffer+1), (q31_t*)pcm.fft);
}

static void pcm_apply_normalization(void) {
	for(uint16_t i = 0; i < pcm_fft_sizes[pcm.api_fft_size]/2; i++) {
		if((i % 8) == 0) {
			coop_task_yield();
		}
		uint64_t value = ((int64_t)pcm.fft[i*2]) * ((int64_t)pcm.fft[i*2]) + ((int64_t)pcm.fft[i*2+1]) * ((int64_t)pcm.fft[i*2+1]);
		pcm.fft[i] = sqrt((double)value);
	}
}

static void pcm_apply_equalization(const uint16_t *equalizations) {
	for(uint16_t i = 0; i < pcm_fft_sizes[pcm.api_fft_size]/2; i++) {
		if((i % 128) == 0) {
			coop_task_yield();
		}
		pcm.fft[i] = ((int64_t)pcm.fft[i])*equalizations[i*pcm_fft_multiplier[pcm.api_fft_size]] / (1<<16);
	}
}

static void pcm_apply_weighting16(const uint16_t *weights) {
	for(uint16_t i = 0; i < pcm_fft_sizes[pcm.api_fft_size]/2; i++) {
		if((i % 128) == 0) {
			coop_task_yield();
		}
		pcm.fft[i] = ((int64_t)pcm.fft[i])*weights[i*pcm_fft_multiplier[pcm.api_fft_size]] / (1<<16);
	}
}

static void pcm_apply_weighting32(const uint32_t *weights) {
	for(uint16_t i = 0; i < pcm_fft_sizes[pcm.api_fft_size]/2; i++) {
		if((i % 128) == 0) {
			coop_task_yield();
		}
		pcm.fft[i] = ((int64_t)pcm.fft[i])*weights[i*pcm_fft_multiplier[pcm.api_fft_size]] / (1<<16);
	}
}

static uint16_t pcm_get_rms_db(void) {
	// Note: We assume here that weighting and equalization has already been applied to fft data
	uint64_t sum_square = 0;
	for(uint16_t i = 0; i < pcm_fft_sizes[pcm.api_fft_size]/2; i++) {
		if((i % 128) == 0) {
			coop_task_yield();
		}
		sum_square += ((int64_t)pcm.fft[i])*((int64_t)pcm.fft[i]);
	}

	// sqrt(rms*2)/2 == sqrt(rms)/sqrt(2)
	double rms = sqrt((double)(sum_square*2)) / (2*lut_hamming_rms);

	uint16_t rms_db = 0;

	switch(pcm.weighting) {
		case PCM_WEIGHTING_A:         rms_db = 10*(20*log10(rms) + 2.0);  break;
		case PCM_WEIGHTING_B:         rms_db = 10*(20*log10(rms) + 0.17); break;
		case PCM_WEIGHTING_C:         rms_db = 10*(20*log10(rms) + 0.06); break;
		case PCM_WEIGHTING_D:         rms_db = 10*20*log10(rms);          break;
		case PCM_WEIGHTING_Z:         rms_db = 10*20*log10(rms);          break;
		case PCM_WEIGHTING_ITU_R_468: rms_db = 10*(20*log10(rms) + 18.2); break;
	}

	return rms_db - 79;
}

void pcm_new_fft_size(void) {
	arm_rfft_init_q31(&pcm.fft_instance, pcm_fft_sizes[pcm.api_fft_size], 0, 1);
	pcm.api_fft_size_new = false;
}

void pcm_tick_task(void) {
	while(true) {
		while(true) {
			coop_task_yield();
			// If there is a new fft size we have to apply it here,
			// but only if there is currently no stream ongoing.
			// If there is a strom ongoing we wait until it finishes.
			if(pcm.api_fft_size_new) {
				if(pcm.api_spectrum_index == 0 && pcm.api_spectrum_cb_index == 0) {
					pcm.weighting = pcm.weighting_new;
					pcm_new_fft_size();
				} else {
					continue;
				}
			}

			// Start new dba sample calculation as soon as buffer is ready
			if(pcm.buffer_ready) {
				// Step 1: Apply window function on time domain data (hamming)
				pcm_remove_dc_offset();
				pcm_apply_hamming_window();
				coop_task_yield();

				break;
			}

		}

		// Step 2: Apply FFT to time domain
		pcm_apply_fft();
		coop_task_yield();

		// Now that we have the FFT we don't need the original buffer anymore
		// and can start to overwrite it again with new data
		pcm.buffer_ready = false;
		ics43432_fill_buffer();

		// Step 3: Apply normalization to frequency domain (combine imaginary and real part of FFT)
		pcm_apply_normalization();
		coop_task_yield();

		// Step 4: Apply equalization to frequency domain
		pcm_apply_equalization(lut_equalization);
		coop_task_yield();

		// Step 5: Apply weighting as configured to frequency domain
		switch(pcm.weighting) {
			case PCM_WEIGHTING_A: pcm_apply_weighting16(lut_a_weighting); break;
			case PCM_WEIGHTING_B: pcm_apply_weighting16(lut_b_weighting); break;
			case PCM_WEIGHTING_C: pcm_apply_weighting16(lut_c_weighting); break;
			case PCM_WEIGHTING_D: pcm_apply_weighting32(lut_d_weighting); break;
			case PCM_WEIGHTING_Z: break;
			case PCM_WEIGHTING_ITU_R_468: pcm_apply_weighting16(lut_itu_r_468_weighting); break;
			default: break;
		}
		coop_task_yield();

		// Step 6: Copy data to buffers
		pcm.api_decibel = pcm_get_rms_db();

		// Only copy to spectrum buffer if there is not currently an ongoing stream.
		if(pcm.api_spectrum_index == 0 && pcm.api_spectrum_cb_index == 0) {
			for(uint16_t i = 0; i < pcm_fft_sizes[pcm.api_fft_size]/2; i++) {
				pcm.api_spectrum[i] = MIN(UINT16_MAX, ABS(pcm.fft[i]));
				pcm.api_spectrum_new_available = true;
			}
		} else {
			// If stream is ongoing we copy to temporary buffer.
			// The temporary buffer is copied to real buffer after stream finishes.
			for(uint16_t i = 0; i < pcm_fft_sizes[pcm.api_fft_size]/2; i++) {
				pcm.api_spectrum_tmp[i] = MIN(UINT16_MAX, ABS(pcm.fft[i]));
				pcm.api_spectrum_tmp_filled = true;
			}
		}
	}
}

void pcm_init(void) {
	pcm.weighting = PCM_WEIGHTING_A;
	pcm.buffer_ready = false;
	pcm.api_spectrum_index = 0;
	pcm.api_spectrum_cb_index = 0;
	pcm.api_spectrum_tmp_filled = false;
	pcm.api_spectrum_period = 0;
	pcm.api_spectrum_new_available = false;
	pcm.api_fft_size = SOUND_PRESSURE_LEVEL_FFT_SIZE_1024;
	pcm.api_fft_size_new = true;
	pcm.state = PCM_STATE_HAMMING_WINDOW;
	ics43432_fill_buffer();

	coop_task_init(&pcm_task, pcm_tick_task);
}

void pcm_tick(void) {
	coop_task_tick(&pcm_task);
}

uint16_t pcm_get_decibel(void) {
	return pcm.api_decibel;
}
