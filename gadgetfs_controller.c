#include "gadgetfs_controller.h"

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include <linux/types.h>
#include <linux/usb/gadgetfs.h>
#include <linux/usb/ch9.h>
#include <linux/hid.h>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>
#include <time.h>
#include <errno.h>

#include "common.h"
#include "usb/usbstring.h"
#include "usb/usb_addon.h"
#include "controller_defines.h"

#define CONFIG_BUFF_EP0 4096
#define CONFIG_BUFF 1024
#define GADGETFS_EVENT_BUFF 10

/*
 * write a USB-descriptor to memory using a char* cursor.
 */
#define append_desc(CUR,DESC)                      \
	{                                              \
		memcpy((CUR), &(DESC), (DESC).bLength);    \
				CUR += (DESC).bLength;             \
	}

/*
 * Configure GadgetFS and open ep0.
 */
static int init_ep0(const char * path, bool highspeed) {
	int ep0;
	ASSERTP(ep0 = open(path, O_RDWR | O_SYNC), error);

	uint8_t config_buf[CONFIG_BUFF_EP0];
	*(uint32_t*)config_buf = 0;
	uint8_t* cur = &config_buf[4];

	//low speed
	append_desc(cur, config);
	append_desc(cur, if_descriptor);
	append_desc(cur, hid_descriptor);
	append_desc(cur, ep_descriptor_in);
	append_desc(cur, ep_descriptor_out);

	if (highspeed) {
		append_desc(cur, config);
		append_desc(cur, if_descriptor);
		append_desc(cur, hid_descriptor);
		append_desc(cur, ep_descriptor_in);
		append_desc(cur, ep_descriptor_out);
	}
	//hardware
	append_desc(cur, device_descriptor);

	size_t size = cur - config_buf;

	ASSERTT(size == write(ep0, config_buf, size), error);
	printf("Configured ep0 %d\n", ep0);
	return ep0;

error:
	close(ep0);
	printf("Error configurating ep0\n");
	return -1;
}

/*
 * Open an endpoint (not ep0).
 */
static int init_ep(char const * path, const struct usb_endpoint_descriptor * desc) {
	uint8_t config_buff[CONFIG_BUFF];

	int ep;
	ASSERTNN(ep = open(path, O_RDWR), err);
	*(uint32_t*) config_buff = 1;
	uint8_t* cur = &config_buff[4];

	append_desc(cur, *desc);
	append_desc(cur, *desc);

	size_t size = cur - config_buff;
	ASSERTT(size == write(ep, config_buff, size), err);
	printf("ep %s %d configured\n", path, ep);
	return ep;
err:
	printf("close\n");
	close(ep);
	return -1;
}

//send input to the console via ep1_in
static void* in_handler(void* arg) {
	gadgetfs_controller_t* controller = (gadgetfs_controller_t*) arg;
	printf("in_thread started\n");
	struct timespec last, now, delta;
	clock_gettime(CLOCK_REALTIME, &now);
	while (!controller->stop) {
		(*controller->get_input)(&controller->state, &delta);
		write(controller->ep1_in, &controller->state, sizeof(controller->state));
		last = now;
		clock_gettime(CLOCK_REALTIME, &now);
		timespec_diff(&last, &now, &delta);
		//printf("send input in %ld ns\n", delta.tv_nsec);
	}
	printf("in_handler closing %d\n", controller->ep1_in);
	close (controller->ep1_in);
	controller->ep1_in = -1;
	return NULL;
}

//recv and discard anything from ep2_out
static void* out_handler(void* arg) {
	gadgetfs_controller_t* controller = (gadgetfs_controller_t*) arg;
	USB_JoystickReport_Output_t dest;
	printf("out_thread started\n");
	while (!controller->stop) {
		int ret = read(controller->ep2_out, &dest, sizeof(dest));
		printf("read %d bytes\n", ret);
		printMem(&dest, sizeof(dest));
	}
	close (controller->ep2_out);
	controller->ep2_out = -1;
	return NULL;
}

/*
 * Everything GadgetFS doesn't handle is handled here.
 * This is mostly identical to the gadgetfs-refrence.
 * NOTE: the worker in and out threads are started in here,
 * once USB is connected and configured
 */
static void handle_setup_request(int fd, struct usb_ctrlrequest* setup, gadgetfs_controller_t* controller) {
	int status, tmp;
	uint8_t buffer[512];

	printf("Setup request %d value %d (%d %d)\n", setup->bRequest, setup->wValue, setup->wValue >> 8, setup->wValue & 0xff);

	switch (setup->bRequest) {
		case USB_REQ_GET_DESCRIPTOR:
			//ASSERT(setup->bRequestType & USB_ENDPOINT_DIR_MASK, == USB_DIR_IN, stall);
			switch (setup->wValue >> 8) {
				case USB_DT_STRING:
					printf("Get String %d (max length %d)\n", setup->wValue & 0xff, setup->wLength);
					ASSERTP(tmp = usb_gadget_get_string(&strings, setup->wValue & 0xff, buffer), missing_data);
					ASSERTNN(status = write (fd, buffer, tmp), e_write);
					ASSERTT(status == tmp, short_write);
					return;
				//case USB_DT_REPORT:
				case 0x22:
					// HID-report
					/*tmp = value & 0x00ff;
					if (verbose > 1)
						printf("... get rpt desc %d, index %04x\n", tmp, setup->wIndex);
					if (tmp != 0 && setup->wIndex != 0)
					goto stall;
					*/
					printf("Get HID Report\n");
					ASSERTNN(status = write(fd, hid_report, hid_report_size), e_write);
					ASSERTT(status == hid_report_size, short_write);
					printf("sent hid report\n");
					return;
				default:
					printf("Get Descriptor %d (UNKNOWN)\n", (setup->wValue >> 8));
					goto stall;
			}
			break;
		case USB_REQ_SET_CONFIGURATION:
			if (setup->bRequestType != USB_DIR_OUT) {
				printf("Bad dir\n");
				goto stall;
			}
			switch (setup->wValue) {
				case CONFIG_VALUE:
					printf("enable configuration %d\n", setup->wValue);
					if (!controller->stop) {
						controller->stop = 1;
						pthread_join(controller->ep1_in_thread, NULL);
						pthread_join(controller->ep2_out_thread, NULL);
						//usleep(200000); // Wait for termination
					}
					if (controller->ep1_in <= 0) {
						ASSERTNN(controller->ep1_in = init_ep(controller->hardware->ep1_in_path, &ep_descriptor_in), stall);
						ASSERTNN(controller->ep2_out = init_ep(controller->hardware->ep2_out_path, &ep_descriptor_out), stall);
					}
					controller->stop = 0;
					// We need two thread for this, because hid has tight
					// timing constraints and we can't do nonblocking IO
					// (either it's not supported or I didn't get it to work)
					pthread_create(&controller->ep1_in_thread, NULL, in_handler, controller);
					pthread_create(&controller->ep2_out_thread, NULL, out_handler, controller);
					break;
				case 0:
					printf("disable configuration\n");
					controller->stop = 1;
					break;
				default:
					printf("enable configuration %d (UNKNOWN)\n", setup->wValue);
					break;
			}
			// Just ACK
			status = read (fd, &status, 0);
			return;
		case USB_REQ_GET_INTERFACE:
			printf("GET_INTERFACE\n");
			buffer[0] = 0;
			write (fd, buffer, 1);
			return;
		case USB_REQ_SET_INTERFACE:
			printf("SET_INTERFACE\n");
			ioctl (controller->ep1_in, GADGETFS_CLEAR_HALT);
			ioctl (controller->ep2_out, GADGETFS_CLEAR_HALT);
			// ACK
			status = read (fd, &status, 0);
			return;
	}
short_write:
	printf("short write\n");
	return;

missing_data:
	printf("Cannot respond, data non existant\n");
	goto stall;
e_write:
	printf("Error writing response\n");
stall:
	printf("Stalled\n");
	// Error
	if (setup->bRequestType & USB_DIR_IN)
		read (fd, &status, 0);
	else
		write (fd, &status, 0);
}

/*
 * We get Configuration-Requests and other control crap via ep0,
 * this function handles those.
 */
static void* ep0_handler(void* arg) {
	gadgetfs_controller_t* controller = (gadgetfs_controller_t*) arg;
	struct usb_gadgetfs_event events[GADGETFS_EVENT_BUFF];

	struct pollfd poll_fds[1];
	poll_fds[0].fd = controller->ep0;
	poll_fds[0].events = POLLIN;
	printf("ep0 thread started\n");

	while (1) {
		ASSERTNN(poll(poll_fds, 1, -1), end);
		int count;
		ASSERTNN(count = read(controller->ep0, &events, sizeof(events)), end);

		count /= sizeof(events[0]);
		printf("ep0: %d event(s)\n", count);

		for (int i=0; i<count; i++) {
			switch (events[i].type) {
				case GADGETFS_CONNECT:
					printf("EP0 CONNECT\n");
					break;
				case GADGETFS_DISCONNECT:
					printf("EP0 DISCONNECT\n");
					break;
				case GADGETFS_SETUP:
					//printf("EP0 SETUP\n");
					handle_setup_request(controller->ep0, &events[i].u.setup, controller);
					break;
				case GADGETFS_NOP:
				case GADGETFS_SUSPEND:
					break;
			}
		}
	}
end:
	return NULL;
}

static void default_get_input(USB_JoystickReport_Input_t* dest, const struct timespec * time) {
	return;
}

int gadfetfs_controller_new(gadgetfs_controller_t* controller, const struct gadgetfs_hardware* hardware, void (*get_input)(USB_JoystickReport_Input_t*, const struct timespec *)) {
	memset(controller, 0, sizeof(*controller));
	controller->hardware = hardware;
	controller->get_input = get_input;
	controller->state = input_neutral;
	controller->get_input = get_input ? get_input : &default_get_input;
	ASSERTNN(controller->ep0 = init_ep0(hardware->ep0_path, hardware->highspeed), err);
	ASSERTZ(pthread_create(&controller->ep0_thread, NULL, ep0_handler, controller), err);
	return 0;

err:
	printf("error in new\n");
	if (controller->ep0 > 0)
		close(controller->ep0);
	return -1;
}
