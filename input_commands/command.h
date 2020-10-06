
#ifndef COMMAND_H
#define COMMAND_H

#include "../controller_defines.h"

typedef struct {
	const struct timespec * last_frametime;
	const USB_JoystickReport_Input_t* last;
	USB_JoystickReport_Input_t* cur;
} CommandArgs;

/*
 * This describes a command.
 * call registerCommand in commands.c with stucts like this.
 */
typedef struct {
	void (*call)(char*);
	void (*frame)(CommandArgs*);
	char name[];
} CommandHook;

#endif
