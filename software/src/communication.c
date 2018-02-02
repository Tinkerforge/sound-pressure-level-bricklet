/* sound-pressure-level-bricklet
 * Copyright (C) 2018 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/utility/callback_value.h"
#include "bricklib2/utility/util_definitions.h"
#include "bricklib2/hal/system_timer/system_timer.h"

#include "pcm.h"

extern const uint16_t pcm_fft_sizes[4];
extern PCM pcm;

CallbackValue callback_value_decibel;

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_GET_DECIBEL: return get_callback_value(message, response, &callback_value_decibel);
		case FID_SET_DECIBEL_CALLBACK_CONFIGURATION: return set_callback_value_callback_configuration(message, &callback_value_decibel);
		case FID_GET_DECIBEL_CALLBACK_CONFIGURATION: return get_callback_value_callback_configuration(message, response, &callback_value_decibel);
		case FID_GET_SPECTRUM_LOW_LEVEL: return get_spectrum_low_level(message, response);
		case FID_SET_SPECTRUM_CALLBACK_CONFIGURATION: return set_spectrum_callback_configuration(message);
		case FID_GET_SPECTRUM_CALLBACK_CONFIGURATION: return get_spectrum_callback_configuration(message, response);
		case FID_SET_CONFIGURATION: return set_configuration(message);
		case FID_GET_CONFIGURATION: return get_configuration(message, response);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}


BootloaderHandleMessageResponse get_spectrum_low_level(const GetSpectrumLowLevel *data, GetSpectrumLowLevel_Response *response) {
	response->header.length = sizeof(GetSpectrumLowLevel_Response);

	response->spectrum_length       = pcm_fft_sizes[pcm.api_fft_size]/2;
	response->spectrum_chunk_offset = pcm.api_spectrum_index;
	uint8_t length = MIN(30, PCM_FFT_SIZE - pcm.api_spectrum_index);

	memcpy(response->spectrum_chunk_data, &pcm.api_spectrum[pcm.api_spectrum_index], 30*2);

	pcm.api_spectrum_index += length;

	if(pcm.api_spectrum_index >= pcm_fft_sizes[pcm.api_fft_size]/2) {
		// If we got new data in the meantime, we copy it now.
		if(pcm.api_spectrum_tmp_filled) {
			memcpy(pcm.api_spectrum, pcm.api_spectrum_tmp, sizeof(uint16_t)*pcm_fft_sizes[pcm.api_fft_size]/2);
			pcm.api_spectrum_tmp_filled = false;
			pcm.api_spectrum_new_available = true;
		}
		pcm.api_spectrum_index = 0;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_spectrum_callback_configuration(const SetSpectrumCallbackConfiguration *data) {
	pcm.api_spectrum_period = data->period;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_spectrum_callback_configuration(const GetSpectrumCallbackConfiguration *data, GetSpectrumCallbackConfiguration_Response *response) {
	response->header.length = sizeof(GetSpectrumCallbackConfiguration_Response);
	response->period        = pcm.api_spectrum_period;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse set_configuration(const SetConfiguration *data) {
	if((data->weighting > SOUND_PRESSURE_LEVEL_WEIGHTING_ITU_R_468) ||
	   (data->fft_size > SOUND_PRESSURE_LEVEL_FFT_SIZE_1024)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	pcm.weighting_new    = data->weighting;
	pcm.api_fft_size     = data->fft_size;
	pcm.api_fft_size_new = true;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_configuration(const GetConfiguration *data, GetConfiguration_Response *response) {
	response->header.length = sizeof(GetConfiguration_Response);
	response->fft_size      = pcm.api_fft_size;
	response->weighting     = pcm.weighting;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


bool handle_decibel_callback(void) {
	return handle_callback_value_callback(&callback_value_decibel, FID_CALLBACK_DECIBEL);
}

bool handle_spectrum_low_level_callback(void) {
	static bool is_buffered = false;
	static SpectrumLowLevel_Callback cb;
	static uint32_t last_send_time = 0;

	if(!is_buffered) {
		if((pcm.api_spectrum_period == 0) || (!pcm.api_spectrum_new_available)) {
			return false;
		}

		if(!system_timer_is_time_elapsed_ms(last_send_time, pcm.api_spectrum_period)) {
			return false;
		}

		if(pcm.api_spectrum_cb_index == 0) {
			last_send_time = system_timer_get_ms();
		}

		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(SpectrumLowLevel_Callback), FID_CALLBACK_SPECTRUM_LOW_LEVEL);

		cb.spectrum_length       = pcm_fft_sizes[pcm.api_fft_size]/2;
		cb.spectrum_chunk_offset = pcm.api_spectrum_cb_index;
		uint8_t length = MIN(30, pcm_fft_sizes[pcm.api_fft_size]/2 - pcm.api_spectrum_cb_index);

		memcpy(cb.spectrum_chunk_data, &pcm.api_spectrum[pcm.api_spectrum_cb_index], 30*2);

		pcm.api_spectrum_cb_index += length;

		if(pcm.api_spectrum_cb_index >= pcm_fft_sizes[pcm.api_fft_size]/2) {
			// If we got new data in the meantime, we copy it now.
			if(pcm.api_spectrum_tmp_filled) {
				memcpy(pcm.api_spectrum, pcm.api_spectrum_tmp, sizeof(uint16_t)*pcm_fft_sizes[pcm.api_fft_size]/2);
				pcm.api_spectrum_tmp_filled = false;
			} else {
				pcm.api_spectrum_new_available = false;
			}
			pcm.api_spectrum_cb_index = 0;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(SpectrumLowLevel_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	callback_value_init(&callback_value_decibel, pcm_get_decibel);;

	communication_callback_init();
}
