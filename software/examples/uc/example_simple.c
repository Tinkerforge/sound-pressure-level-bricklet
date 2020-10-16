// This example is not self-contained.
// It requres usage of the example driver specific to your platform.
// See the HAL documentation.

#include "bindings/hal_common.h"
#include "bindings/bricklet_sound_pressure_level.h"

#define UID "XYZ" // Change XYZ to the UID of your Sound Pressure Level Bricklet

void check(int rc, const char* msg);

void example_setup(TF_HalContext *hal);
void example_loop(TF_HalContext *hal);


static TF_SoundPressureLevel spl;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_sound_pressure_level_create(&spl, UID, hal), "create device object");

	// Get current decibel
	uint16_t decibel;
	check(tf_sound_pressure_level_get_decibel(&spl, &decibel), "get decibel");

	tf_hal_printf("Decibel: %d 1/%d dB(A)\n", decibel, 10);
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
