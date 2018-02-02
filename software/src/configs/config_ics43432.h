/* sound-pressure-level-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config_ics43432.h: All configurations for ICS43432 mems microphone
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

#ifndef CONFIG_ICS43432_H
#define CONFIG_ICS43432_H

#include "xmc_gpio.h"
#include "xmc_common.h"
#include "xmc_scu.h"

#define ICS43432_USIC_CHANNEL  USIC0_CH0
#define ICS43432_USIC          XMC_I2S0_CH0

#define ICS43432_WS_PIN        P1_4
#define ICS43432_WS_PIN_AF     (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6 | P1_4_AF_U0C0_SELO0)

#define ICS43432_SCK_PIN       P0_14
#define ICS43432_SCK_PIN_AF    (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7 | P0_14_AF_U0C0_SCLKOUT)

#define ICS43432_SD_PIN        P0_15
#define ICS43432_SD_INPUT      XMC_USIC_CH_INPUT_DX0
#define ICS43432_SD_SOURCE     0b001 // DX0B

#define ICS43432_SERVICE_REQUEST_SD  2  // receive
#define ICS43432_IRQ_SD              11
#define ICS43432_IRQ_SD_PRIORITY     0
#define ICS43432_IRQCTRL_SD          XMC_SCU_IRQCTRL_USIC0_SR2_IRQ11

#endif
