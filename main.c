
#include <stdio.h>

#include "common.h"
#include "usb/usb_addon.h"
#include "gadgetfs_controller.h"
#include "input_generator.h"

int main() {
	printf("in main\n");
	struct gadgetfs_hardware hardware = {
		.highspeed = 1,
		.ep0_path = "/dev/gadget/fe980000.usb",
		.ep1_in_path = "/dev/gadget/ep1in",
		.ep2_out_path = "/dev/gadget/ep2out"
	};
	gadgetfs_controller_t controller;
	ASSERTZ(initInputGenerator(), err);
	initDescriptors();
	ASSERTZ(gadfetfs_controller_new(&controller, &hardware, &getInput), err);
	gadgetfs_controller_wait(&controller);
	return 0;
err:
	printf("top level error in main\n");
	return -1;
}
