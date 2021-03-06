/* sound-pressure-level-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * main.c: Initialization for Sound Pressure Level Bricklet
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

#include <stdio.h>
#include <stdbool.h>

#include "configs/config.h"

#include "bricklib2/bootloader/bootloader.h"
#include "bricklib2/hal/system_timer/system_timer.h"
#include "bricklib2/logging/logging.h"
#include "communication.h"
#include "pcm.h"
#include "ics43432.h"

int main(void) {
	logging_init();
	logd("Start Sound Pressure Level Bricklet\n\r");

	communication_init();
	ics43432_init();
	pcm_init();

	while(true) {
		bootloader_tick();
		communication_tick();
		ics43432_tick();
		pcm_tick();
	}
}
