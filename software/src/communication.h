/* sound-pressure-level-bricklet
 * Copyright (C) 2018 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.h: TFP protocol message handling
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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/bootloader/bootloader.h"

// Default functions
BootloaderHandleMessageResponse handle_message(const void *data, void *response);
void communication_tick(void);
void communication_init(void);

// Constants
#define SOUND_PRESSURE_LEVEL_THRESHOLD_OPTION_OFF 'x'
#define SOUND_PRESSURE_LEVEL_THRESHOLD_OPTION_OUTSIDE 'o'
#define SOUND_PRESSURE_LEVEL_THRESHOLD_OPTION_INSIDE 'i'
#define SOUND_PRESSURE_LEVEL_THRESHOLD_OPTION_SMALLER '<'
#define SOUND_PRESSURE_LEVEL_THRESHOLD_OPTION_GREATER '>'

#define SOUND_PRESSURE_LEVEL_FFT_SIZE_128 0
#define SOUND_PRESSURE_LEVEL_FFT_SIZE_256 1
#define SOUND_PRESSURE_LEVEL_FFT_SIZE_512 2
#define SOUND_PRESSURE_LEVEL_FFT_SIZE_1024 3

#define SOUND_PRESSURE_LEVEL_WEIGHTING_A 0
#define SOUND_PRESSURE_LEVEL_WEIGHTING_B 1
#define SOUND_PRESSURE_LEVEL_WEIGHTING_C 2
#define SOUND_PRESSURE_LEVEL_WEIGHTING_D 3
#define SOUND_PRESSURE_LEVEL_WEIGHTING_Z 4
#define SOUND_PRESSURE_LEVEL_WEIGHTING_ITU_R_468 5

#define SOUND_PRESSURE_LEVEL_BOOTLOADER_MODE_BOOTLOADER 0
#define SOUND_PRESSURE_LEVEL_BOOTLOADER_MODE_FIRMWARE 1
#define SOUND_PRESSURE_LEVEL_BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT 2
#define SOUND_PRESSURE_LEVEL_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT 3
#define SOUND_PRESSURE_LEVEL_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT 4

#define SOUND_PRESSURE_LEVEL_BOOTLOADER_STATUS_OK 0
#define SOUND_PRESSURE_LEVEL_BOOTLOADER_STATUS_INVALID_MODE 1
#define SOUND_PRESSURE_LEVEL_BOOTLOADER_STATUS_NO_CHANGE 2
#define SOUND_PRESSURE_LEVEL_BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT 3
#define SOUND_PRESSURE_LEVEL_BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT 4
#define SOUND_PRESSURE_LEVEL_BOOTLOADER_STATUS_CRC_MISMATCH 5

#define SOUND_PRESSURE_LEVEL_STATUS_LED_CONFIG_OFF 0
#define SOUND_PRESSURE_LEVEL_STATUS_LED_CONFIG_ON 1
#define SOUND_PRESSURE_LEVEL_STATUS_LED_CONFIG_SHOW_HEARTBEAT 2
#define SOUND_PRESSURE_LEVEL_STATUS_LED_CONFIG_SHOW_STATUS 3

// Function and callback IDs and structs
#define FID_GET_DECIBEL 1
#define FID_SET_DECIBEL_CALLBACK_CONFIGURATION 2
#define FID_GET_DECIBEL_CALLBACK_CONFIGURATION 3
#define FID_GET_SPECTRUM_LOW_LEVEL 5
#define FID_SET_SPECTRUM_CALLBACK_CONFIGURATION 6
#define FID_GET_SPECTRUM_CALLBACK_CONFIGURATION 7
#define FID_SET_CONFIGURATION 9
#define FID_GET_CONFIGURATION 10

#define FID_CALLBACK_DECIBEL 4
#define FID_CALLBACK_SPECTRUM_LOW_LEVEL 8

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetSpectrumLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint16_t spectrum_length;
	uint16_t spectrum_chunk_offset;
	uint16_t spectrum_chunk_data[30];
} __attribute__((__packed__)) GetSpectrumLowLevel_Response;

typedef struct {
	TFPMessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) SetSpectrumCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetSpectrumCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) GetSpectrumCallbackConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	uint16_t spectrum_length;
	uint16_t spectrum_chunk_offset;
	uint16_t spectrum_chunk_data[30];
} __attribute__((__packed__)) SpectrumLowLevel_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t fft_size;
	uint8_t weighting;
} __attribute__((__packed__)) SetConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t fft_size;
	uint8_t weighting;
} __attribute__((__packed__)) GetConfiguration_Response;


// Function prototypes
BootloaderHandleMessageResponse get_spectrum_low_level(const GetSpectrumLowLevel *data, GetSpectrumLowLevel_Response *response);
BootloaderHandleMessageResponse set_spectrum_callback_configuration(const SetSpectrumCallbackConfiguration *data);
BootloaderHandleMessageResponse get_spectrum_callback_configuration(const GetSpectrumCallbackConfiguration *data, GetSpectrumCallbackConfiguration_Response *response);
BootloaderHandleMessageResponse set_configuration(const SetConfiguration *data);
BootloaderHandleMessageResponse get_configuration(const GetConfiguration *data, GetConfiguration_Response *response);

// Callbacks
bool handle_decibel_callback(void);
bool handle_spectrum_low_level_callback(void);

#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 2
#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_decibel_callback, \
	handle_spectrum_low_level_callback, \


#endif
