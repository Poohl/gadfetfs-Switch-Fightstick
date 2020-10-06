
#include "input_generator.h"
#include "common.h"

/*
 * Dummy input toggeling A every second.
 */

#define TIME_S 1

static USB_JoystickReport_Input_t last;
static struct timespec elapsed;

int initInputGenerator() {
	last = input_neutral;
	elapsed.tv_sec = elapsed.tv_nsec = 0;
	return 0;
}

void getInput(USB_JoystickReport_Input_t* dest, const struct timespec * last_frametime) {
	timespec_add(&elapsed, last_frametime, &elapsed);
	if (elapsed.tv_sec >= TIME_S) {
		last.Button ^= SWITCH_A;
		elapsed.tv_sec = elapsed.tv_nsec = 0;
	}
	*dest = last;
}
