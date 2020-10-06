
#include "util.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "common.h"

/*
 * arbitry list of all button names
 */
static const char * const buttons[] = {
	"y", "b", "a", "x",
	"l", "r", "zl", "zr",
	"plus", "minus", "sleft", "sright", "home", "capture",
	"dup", "dright", "ddown", "dleft", NULL
};

int buttonToIndex(char* buff) {
	for (char* p = buff; *p; ++p) *p = tolower(*p);
	return prefixStrSelect(buttons, buff);
}

const char * indexToButtonStr(int i) {
	if (i < 0 || i > sizeof(buttons) / sizeof(buttons[0]))
		return NULL;
	else
		return buttons[i];
}

uint8_t fillHat(uint8_t h) {
	h &= 0xF;
	return (h << 4) | h;
}

uint8_t buttonToHatMatrix[16] = {
	0x8,
	0x6,
	0x4,
	0x5,
	0x2,
	0xF,
	0x3,
	0xF,
	0x0,
	0x7,
	0xF,
	0xF,
	0x1,
	0xF,
	0xF,
	0xF
};

uint8_t hatToButtonMatrix[16] = {
	0b1000,
	0b1100,
	0b0100,
	0b0110,
	0b0010,
	0b0011,
	0b0001,
	0b1001,
	0b0000,
	0b0000,
	0b0000,
	0b0000,
	0b0000,
	0b0000,
	0b0000,
	0b0000
};

void addInputs(USB_JoystickReport_Input_t* a, const USB_JoystickReport_Input_t* b) {
	a->Button |= b->Button;
	a->HAT = fillHat(buttonToHat(hatToButton(a->HAT) | hatToButton(b->HAT)));
	a->LX = clamp(mapRange(a->LX + b->LX, 0, 255+255, 0, 255), 0, 255);
	a->LY = clamp(mapRange(a->LY + b->LY, 0, 255+255, 0, 255), 0, 255);
	a->RX = clamp(mapRange(a->RX + b->RX, 0, 255+255, 0, 255), 0, 255);
	a->RY = clamp(mapRange(a->RY + b->RY, 0, 255+255, 0, 255), 0, 255);
}
