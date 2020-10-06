
#ifndef GADGETFS_CONTROLLER_H
#define GADGETFS_CONTROLLER_H

/*
 * The core file
 * This handles creation and emulation of the gadgetfs-gadgets.
 */

#include <pthread.h>
#include <stdbool.h>

#include "common.h"
#include "usb/usb_addon.h"
#include "controller_defines.h"

typedef struct {
	char connected;
	char configured;
	char stop;
	int ep0, ep1_in, ep2_out;
	pthread_t ep0_thread, ep1_in_thread, ep2_out_thread;
	const struct gadgetfs_hardware* hardware;
	void (*get_input)(USB_JoystickReport_Input_t*, const struct timespec *);
	USB_JoystickReport_Input_t state;
} gadgetfs_controller_t;

/*
 * Create and start a new Controller on the given hardware and using the given
 * callback for getting input.
 * If get_input is null, you can just write to controller->state directly
 */
int gadfetfs_controller_new(gadgetfs_controller_t* controller, const struct gadgetfs_hardware* hardware, void (*get_input)(USB_JoystickReport_Input_t*, const struct timespec *));

/*
 * Wait for termination of the controller.
 */
#define gadgetfs_controller_wait(CONTROLLER) \
	pthread_join((CONTROLLER)->ep0_thread, NULL)

#endif
