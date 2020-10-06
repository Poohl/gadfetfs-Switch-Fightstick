
#include "mariomaker2.h"

#include "common.h"
#include "util.h"

static bool inv_sprint = 0;
static bool world_spin = 0;

static const char* const mariomaker_commands[] = {"sprint","inv_sprint", "3d_spin", NULL};

static void mariomaker_call(char* args) {
	switch (prefixStrSelect(mariomaker_commands, args)) {
		case 0:
		case 1:
			inv_sprint = !inv_sprint;
			break;
		case 2:
			world_spin = !world_spin;
			break;
	}
}

static void mariomaker_frame(CommandArgs* args) {
	if (inv_sprint)
		args->cur->Button ^= SWITCH_Y | SWITCH_X;
	if (world_spin && (args->cur->Button & (SWITCH_L | SWITCH_R)) && !(args->last->Button & (SWITCH_L | SWITCH_R))) {
		args->cur->HAT = fillHat(buttonToHat(hatToButton(args->cur->HAT) | 0b1000));
		args->cur->Button |= SWITCH_B;
	}
}

CommandHook mariomaker_hook = {&mariomaker_call, &mariomaker_frame, "mariomaker"};
CommandHook mm_hook = {&mariomaker_call, &mariomaker_frame, "mm"};
