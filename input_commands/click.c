
#include "click.h"

#include <stdio.h>

#include "util.h"

static USB_JoystickReport_Input_t buttons[] =  {
	{SWITCH_Y, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_B, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_A, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_X, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_L, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_R, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_ZL, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_ZR, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_SELECT, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_START, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_LCLICK, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_RCLICK, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_HOME, 0xFF, 0, 0, 0, 0, 0},
	{SWITCH_CAPTURE, 0xFF, 0, 0, 0, 0, 0},
	{0, 0x0, 0, 0, 0, 0, 0},
	{0, 0x22, 0, 0, 0, 0, 0},
	{0, 0x44, 0, 0, 0, 0, 0},
	{0, 0x66, 0, 0, 0, 0, 0}
};

static USB_JoystickReport_Input_t out;
static char sent = 1;

static void click_call(char* args) {
	out = input_neutral;
	int b;
	printf("click");
	while (*args) {
		b = buttonToIndex(args);
		if (b >= 0) {
			addInputs(&out, &buttons[b]);
			printf(" %s", indexToButtonStr(b));
		}
		while (*args && *args != ' ')
			++args;
		args += (*args == ' ');
	}
	printf("\n");
	sent = 0;
}

static void click_frame(CommandArgs* args) {
	if (sent < CLICK_FRAMES) {
		addInputs(args->cur, &out);
		++sent;
	}
}

CommandHook click_hook = {&click_call, &click_frame, "click"};
