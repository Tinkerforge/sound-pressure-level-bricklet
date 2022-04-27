// This example is not self-contained.
// It requires usage of the example driver specific to your platform.
// See the HAL documentation.

#include "src/bindings/hal_common.h"
#include "src/bindings/bricklet_sound_pressure_level.h"

void check(int rc, const char *msg);
void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);

static TF_SoundPressureLevel spl;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_sound_pressure_level_create(&spl, NULL, hal), "create device object");

	// Get current decibel
	uint16_t decibel;
	check(tf_sound_pressure_level_get_decibel(&spl, &decibel), "get decibel");

	tf_hal_printf("Decibel: %d 1/%d dB(A)\n", decibel, 10);
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
