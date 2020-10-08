
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"
#include "util.h"
#include "command.h"

static int file_fd = 0;
static int frames = 0;
static int ms = 0;

#define MODE_TIME_MASK 1
#define MODE_TIME_REAL 1
#define MODE_TIME_FRAMES 0

struct action {
	USB_JoystickReport_Input_t input; // 8 byte
	union { // 4 byte
		int frames;
		int ms;
	};
	unsigned char mode; // 1 byte
	/* prolly 3 byte padding, idc */
};

static struct action current_action;

static void tas_call(char* args) {
	ASSERTP(file_fd = open(args, O_RDONLY), err);
	return;
err:
	printf("could not open file %s\n", args);
	file_fd = 0;
}

static void tas_frame(CommandArgs* args) {
	if (!file_fd)
		return;
	ms += args->last_frametime->tv_nsec / 1000000;
	++frames;
	if ((current_action.mode & MODE_TIME_MASK) == MODE_TIME_REAL ? ms > current_action.ms : frames > current_action.frames) {
		ASSERTT(read(file_fd, &current_action, 13) == 13, done);
		printf("send for %d %s (after %ld ns)\n", current_action.frames, (current_action.mode & MODE_TIME_MASK) == MODE_TIME_REAL ? "ms" : "frames", args->last_frametime->tv_nsec);
		printMem(&current_action.input, sizeof(current_action.input));
		frames = 0;
		ms = 0;
	}
	*args->cur = current_action.input;
	return;
done:
	close(file_fd);
	file_fd = 0;
	return;
}

CommandHook tas_hook = {&tas_call, &tas_frame, "tas"};
