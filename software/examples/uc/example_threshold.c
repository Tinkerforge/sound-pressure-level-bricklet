// This example is not self-contained.
// It requres usage of the example driver specific to your platform.
// See the HAL documentation.

#include "bindings/hal_common.h"
#include "bindings/bricklet_sound_pressure_level.h"

#define UID "XYZ" // Change XYZ to the UID of your Sound Pressure Level Bricklet

void check(int rc, const char* msg);

void example_setup(TF_HalContext *hal);
void example_loop(TF_HalContext *hal);


// Callback function for decibel callback
static void decibel_handler(TF_SoundPressureLevel *device, uint16_t decibel,
                            void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("Decibel: %d 1/%d dB(A)\n", decibel, 10.0);
}

static TF_SoundPressureLevel spl;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_sound_pressure_level_create(&spl, UID, hal), "create device object");

	// Register decibel callback to function decibel_handler
	tf_sound_pressure_level_register_decibel_callback(&spl,
	                                                  decibel_handler,
	                                                  NULL);

	// Configure threshold for decibel "greater than 60 dB(A)"
	// with a debounce period of 1s (1000ms)
	tf_sound_pressure_level_set_decibel_callback_configuration(&spl, 1000, false, '>', 60*10, 0);
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
