

#include "wii.h"

#include <xwiimote.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>

#include "util.h"
#include "common.h"
#include "../controller_defines.h"

typedef struct {
	uint from;
	uint to;
} ButtonMap;

static ButtonMap vertical[] = {
		{XWII_KEY_A, SWITCH_A},
		{XWII_KEY_B, SWITCH_B},
		{XWII_KEY_PLUS, SWITCH_START},
		{XWII_KEY_MINUS, SWITCH_SELECT},
		{XWII_KEY_ONE,SWITCH_X},
		{XWII_KEY_TWO,SWITCH_Y},
		{XWII_KEY_HOME,SWITCH_HOME},
		{XWII_KEY_Z,SWITCH_ZR},
		{XWII_KEY_C,SWITCH_R}
};

static ButtonMap horizontal[] = {
		{XWII_KEY_A, SWITCH_A},
		{XWII_KEY_B, SWITCH_L},
		{XWII_KEY_PLUS, SWITCH_START},
		{XWII_KEY_MINUS, SWITCH_SELECT},
		{XWII_KEY_ONE,SWITCH_Y},
		{XWII_KEY_TWO,SWITCH_A},
		{XWII_KEY_HOME,SWITCH_HOME},
		{XWII_KEY_Z,SWITCH_ZL},
		{XWII_KEY_C,SWITCH_R}
};

static ButtonMap* map = NULL;

static char enabled = 0;
static struct xwii_monitor* monitor;
static struct xwii_iface* wiimote;
static int fd;

static USB_JoystickReport_Input_t state;

static void wii_call(char* args) {
	if (*args) {
		if (*args == 'h') {
			map = horizontal;
		} else if (*args == 'v') {
			map = vertical;
		} else
			return;
	} else {
		map = horizontal;
	}
	if (!enabled) {
		state = input_neutral;
		ASSERT(monitor = xwii_monitor_new(1, 0), != NULL, err);
		printf("monitor open\n");
		char* wiimote_path = xwii_monitor_poll(monitor);
		printf("Wiimote at %s\n", wiimote_path);
		ASSERTZ(xwii_iface_new(&wiimote, wiimote_path), err);
		printf("got interface\n");
		xwii_iface_open(wiimote, XWII_IFACE_CORE | XWII_IFACE_NUNCHUK);
		ASSERT(xwii_iface_opened(wiimote) & XWII_IFACE_CORE, == XWII_IFACE_CORE, err);
		ASSERTZ(xwii_iface_watch(wiimote, true), err);
		printf("opened interface\n");
		ASSERTNN(fd = xwii_iface_get_fd(wiimote), err);
		printf("got fd for wiimote %d\n", fd);
	} else {
		xwii_monitor_unref(monitor);
		xwii_iface_close(wiimote, XWII_IFACE_CORE | XWII_IFACE_NUNCHUK);
		xwii_iface_unref(wiimote);
	}
	enabled = !enabled;
	return;

err:
	printf("Error opening wiimote\n");
	return;
}

static void wii_frame(CommandArgs* args) {
	if (!enabled)
		return;
	struct xwii_event ev;
	struct pollfd poll_fds[1];
	poll_fds[0].fd = fd;
	poll_fds[0].events = POLLIN;
	while (poll(poll_fds, 1, 1) > 0) {
		if (poll_fds[0].revents & ~POLLIN) {
			continue;
		}
		if(xwii_iface_dispatch(wiimote, &ev,sizeof(ev)) < 0) {
			continue;
		}
		int i;
		switch(ev.type) {
			case XWII_EVENT_KEY:
			case XWII_EVENT_NUNCHUK_KEY:
				printf("key\n");
				for (i = 0; i < 9 && map[i].from != ev.v.key.code; ++i);
				if (i < 9) {
					state.Button = setMaskTo(state.Button, map[i].to, ev.v.key.state);
					break;
				}
				state.HAT &= 0xF;
				if (state.HAT < 0x8 && map == horizontal) {
					state.HAT = mathMod(((state.HAT & 0xF) + 2), 8);
				}
				switch (ev.v.key.code) {
					case XWII_KEY_DOWN:
						state.HAT = buttonToHat(setMaskTo(hatToButton(state.HAT), 0b0010, ev.v.key.state));
						break;
					case XWII_KEY_RIGHT:
						state.HAT = buttonToHat(setMaskTo(hatToButton(state.HAT), 0b0100, ev.v.key.state));
						break;
					case XWII_KEY_LEFT:
						state.HAT = buttonToHat(setMaskTo(hatToButton(state.HAT), 0b0001, ev.v.key.state));
						break;
					case XWII_KEY_UP:
						state.HAT = buttonToHat(setMaskTo(hatToButton(state.HAT), 0b1000, ev.v.key.state));
						break;
				}
				if (state.HAT >= 0x8)
					state.HAT = 0x8;
				else if (map == horizontal) {
					state.HAT = mathMod(state.HAT - 2, 8);
				}
				state.HAT = fillHat(state.HAT);
				if (state.HAT == 0x88)
					state.HAT = 0xFF;
				printf("HAT: %x\n", state.HAT);
				break;
			case XWII_EVENT_NUNCHUK_MOVE:
				//printf("Nunchuk move %d %d %d\n", ev.v.abs[0].x,ev.v.abs[0].y,ev.v.abs[0].z);
				state.LX = clamp(mapRange((int) ev.v.abs[0].x, -100, 100, 0, 255), 0, 255);
				state.LY = clamp(mapRange((int) ev.v.abs[0].y, -100, 100, 255, 0), 0, 255);
				break;
			case XWII_EVENT_WATCH:
				printf("Wii Harware setup event\n");
				xwii_iface_open(wiimote, xwii_iface_available(wiimote) & (XWII_IFACE_CORE | XWII_IFACE_NUNCHUK));
				break;
			default:
				printf("Wii unknown event %d\n", ev.type);
		}
	}
	*args->cur = state;
	return;
err:
	memset(args->cur, 0, sizeof(*args->cur));
	args->cur->HAT = 0x88;
	return;
}

CommandHook wii_hook = {&wii_call, &wii_frame, "wii"};
