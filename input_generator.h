
#ifndef INPUT_GENERATOR_H
#define INPUT_GENERATOR_H

/*
 * A headerfile to implement, if you want to be compatible with the simple
 * main and instead of writing commands just want to overtake all the input
 * generation yourself.
 */

#include "controller_defines.h"

#include <time.h>

/*
 * Called once before startup
 * return 0 on success
 */
int initInputGenerator();

/*
 * Called each time the Contoller-emulator requires nwe input.
 * dest: the place to put the next input to be sent
 * last_frametime: the time since the last call to getInput
 */
void getInput(USB_JoystickReport_Input_t* dest, const struct timespec * last_frametime);

#endif
