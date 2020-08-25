#include "bindings/hal_common.h"
#include "bindings/bricklet_sound_pressure_level.h"

#define UID "XYZ" // Change XYZ to the UID of your Sound Pressure Level Bricklet

void check(int rc, const char* msg);

// Callback function for decibel callback
void decibel_handler(TF_SoundPressureLevel *device, uint16_t decibel, void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("Decibel: %d 1/%d dB(A)\n", decibel, 10.0);
}

TF_SoundPressureLevel spl;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_sound_pressure_level_create(&spl, UID, hal), "create device object");


	// Register decibel callback to function decibel_handler
	tf_sound_pressure_level_register_decibel_callback(&spl,
	                                                 decibel_handler,
	                                                 NULL);

	// Set period for decibel callback to 1s (1000ms) without a threshold
	tf_sound_pressure_level_set_decibel_callback_configuration(&spl, 1000, false, 'x', 0, 0);

}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
