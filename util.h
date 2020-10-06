
#ifndef COMMANDS_UTIL_H
#define COMMANDS_UTIL_H

#include <stdint.h>
#include "controller_defines.h"

// order is y b a x l r zl zr + - sl sr h c dup dright ddown dleft
int buttonToIndex(char* buff);

const char* indexToButtonStr(int i);

#define packButtons(D,C,B,A) \
	((bool) D << 3) | ((bool) C << 2) | ((bool) B << 1) | ((bool) A)

uint8_t fillHat(uint8_t h);

extern uint8_t buttonToHatMatrix[];

#define buttonToHat(BUTTON) \
	buttonToHatMatrix[(BUTTON) & 0xF]

extern uint8_t hatToButtonMatrix[];

#define hatToButton(HAT) \
	hatToButtonMatrix[(HAT) & 0xF]

void addInputs(USB_JoystickReport_Input_t* a, const USB_JoystickReport_Input_t* b);

#endif
