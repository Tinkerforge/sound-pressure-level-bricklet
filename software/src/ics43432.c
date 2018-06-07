/* sound-pressure-level-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ics43432.c: ICS43432 driver
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

#include "ics43432.h"

#include "configs/config_ics43432.h"
#include "bricklib2/hal/system_timer/system_timer.h"

#include "xmc_i2s.h"
#include "xmc_gpio.h"

#include "pcm.h"

#define ics43432_sd_irq_handler  IRQ_Hdlr_11
extern uint16_t pcm_fft_sizes[4];
extern PCM pcm;

volatile int32_t *ics43432_buffer = pcm.buffer;
int32_t *const ics43432_buffer_start = pcm.buffer;
volatile int32_t *ics43432_buffer_end = pcm.buffer + PCM_BUFFER_SIZE + 1;

void __attribute__((optimize("unroll-loops"))) __attribute__((optimize("-O3"))) __attribute__ ((section (".ram_code"))) ics43432_sd_irq_handler(void) {
	for(uint8_t i = 0; i < 16; i++) {
		const uint32_t value = ICS43432_USIC->OUTR;
		if(!(value & 1 << 20)) { // Check OUTR.RCI[0] == 1 => left channel
			if(value & (1 << 16)) { // Check OUTR.RCI[0] == 1 => first data of frame
				*ics43432_buffer = (value & 0xFFFF) << 16;
			} else {
				*ics43432_buffer |= (value & 0xFFFF);
				ics43432_buffer++;
			}
		}
	}

	if(ics43432_buffer >= ics43432_buffer_end) {
		pcm.buffer_ready = true;
		NVIC_DisableIRQ(ICS43432_IRQ_SD);
	}
}

void ics43432_init(void) {
	// WS pin configuration
	const XMC_GPIO_CONFIG_t ws_pin_config = {
		.mode             = ICS43432_WS_PIN_AF,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
	};

	// SCK pin configuration
	const XMC_GPIO_CONFIG_t sck_pin_config = {
		.mode             = ICS43432_SCK_PIN_AF,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
	};

	// SD pin configuration
	const XMC_GPIO_CONFIG_t sd_pin_config = {
		.mode             = XMC_GPIO_MODE_INPUT_TRISTATE,
		.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
	};


	// Configure  pins
	XMC_GPIO_Init(ICS43432_WS_PIN, &ws_pin_config);
	XMC_GPIO_Init(ICS43432_SCK_PIN, &sck_pin_config);
	XMC_GPIO_Init(ICS43432_SD_PIN, &sd_pin_config);

	const XMC_I2S_CH_CONFIG_t config = {
		.baudrate = 40960*32*2, // sample rate * 2 * bits per sample
		.data_bits = 16,
		.frame_length = 32,
		.data_delayed_sclk_periods = 1,
		.wa_inversion = XMC_I2S_CH_WA_POLARITY_DIRECT,
		.bus_mode = XMC_I2S_CH_BUS_MODE_MASTER
	};

	// Initialize USIC channel in I2S mode
	XMC_I2S_CH_Init(ICS43432_USIC, &config);
	ICS43432_USIC->RBCTR |= USIC_CH_RBCTR_RCIM_Msk;

	// Set MSB data shift direction
	XMC_I2S_CH_SetBitOrderMsbFirst(ICS43432_USIC);

	// Set input source for input stage dx0 (receive pin)
	XMC_I2S_CH_SetInputSource(ICS43432_USIC, ICS43432_SD_INPUT, ICS43432_SD_SOURCE);

	// Configure the clock polarity and clock delay
	XMC_USIC_CH_ConfigureShiftClockOutput(ICS43432_USIC, XMC_USIC_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_0_DELAY_DISABLED, XMC_USIC_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK);


	XMC_USIC_CH_EnableInputDigitalFilter(ICS43432_USIC, XMC_USIC_CH_INPUT_DX0); // ?

	// Set the service request line for the Data Lost, Baud Rate Generator and I2S protocol events
	XMC_USIC_CH_SetInterruptNodePointer(ICS43432_USIC, XMC_USIC_CH_INTERRUPT_NODE_POINTER_PROTOCOL, 0);


	// Configure the receive FIFO
	XMC_USIC_CH_RXFIFO_Configure(ICS43432_USIC, 32, XMC_USIC_CH_FIFO_SIZE_32WORDS, 16);

	XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(ICS43432_USIC, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD, ICS43432_SERVICE_REQUEST_SD);

	XMC_I2S_CH_EnableEvent(ICS43432_USIC, XMC_I2S_CH_EVENT_STANDARD_RECEIVE);
	XMC_USIC_CH_RXFIFO_EnableEvent(ICS43432_USIC, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD);

	// Set priority and enable NVIC node for receive interrupt
	NVIC_SetPriority(ICS43432_IRQ_SD, ICS43432_IRQ_SD_PRIORITY);
	XMC_SCU_SetInterruptControl(ICS43432_IRQ_SD, ICS43432_IRQCTRL_SD);
	NVIC_DisableIRQ(ICS43432_IRQ_SD);

	XMC_I2S_CH_EnableMasterClock(ICS43432_USIC);
	XMC_I2S_CH_Start(ICS43432_USIC);
}

void ics43432_fill_buffer(void) {
	memset(pcm.buffer, 0, PCM_BUFFER_SIZE);
	ics43432_buffer = ics43432_buffer_start;
	ics43432_buffer_end = pcm.buffer + pcm_fft_sizes[pcm.api_fft_size] + 1;

	XMC_USIC_CH_RXFIFO_Flush(ICS43432_USIC);
	NVIC_EnableIRQ(ICS43432_IRQ_SD);
}

void ics43432_tick(void) {
	if(NVIC_GetEnableIRQ(ICS43432_IRQ_SD)) {
		// If the RX buffer is filled up, something went wrong.
		// In this case we just flush the buffer to be sure that we get
		// another fill level change from 15 to 16 that generates the i2s interrupt
		if(XMC_USIC_CH_RXFIFO_IsFull(ICS43432_USIC)) {
			XMC_USIC_CH_RXFIFO_Flush(ICS43432_USIC);
		}
	}
}