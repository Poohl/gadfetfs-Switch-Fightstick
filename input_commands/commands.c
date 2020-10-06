#include "../input_generator.h"

#include <time.h>
#include <pthread.h>
#include <stdio.h>

#include "command.h"
#include "../common.h"
#include "../controller_defines.h"

#ifdef CLICK
#include "click.h"
#endif
#ifdef WII
#include "wii.h"
#endif
#ifdef MARIOMAKER2
#include "mariomaker2.h"
#endif


#define COMMAND_COUNT 10

static CommandHook* commands[COMMAND_COUNT] = {NULL};
static const char* command_names[COMMAND_COUNT + 1] = {NULL};

static void registerCommand(CommandHook* command) {
	uint i;
	for (i = 0; i < COMMAND_COUNT && commands[i]; ++i);
	if (i == COMMAND_COUNT)
		return;
	commands[i] = command;
	command_names[i] = command->name;
}

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
//static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_t thread;

static USB_JoystickReport_Input_t last;
//static struct timeval frametime;

static void* command_caller(void * shit) {
	printf("terminal started\n");
	char* in_line = NULL;
	size_t in_len = 0;
	while (1) {
		getline(&in_line, &in_len, stdin);
		in_line[strlen(in_line) - 1] = 0;
		pthread_mutex_lock(&lock);
		int i = prefixStrSelect(command_names, in_line);
		int j;
		for (j = 0; in_line[j] && in_line[j] != ' '; ++j);
		printf("Got command %s %d\n", in_line, i);
		if (i >= 0 && commands[i]) {
			(*commands[i]->call)(in_line + j + (in_line[j] == ' '));
		} else {
			printf("unknown command\n");
		}
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}

int initInputGenerator() {
	last = input_neutral;
	#ifdef CLICK
	registerCommand(&click_hook);
	#endif
	#ifdef WII
	registerCommand(&wii_hook);
	#endif
	#ifdef MARIOMAKER2
	registerCommand(&mariomaker_hook);
	registerCommand(&mm_hook);
	#endif
	pthread_create(&thread, NULL, command_caller, NULL);
	return 0;
}

void getInput(USB_JoystickReport_Input_t* dest, const struct timespec * last_frametime) {
	USB_JoystickReport_Input_t dest_buff = input_neutral;
	CommandArgs args = {last_frametime, &last, &dest_buff};
	pthread_mutex_lock(&lock);
	for (int i = 0; i < COMMAND_COUNT && commands[i]; ++i) {
		(*commands[i]->frame)(&args);
	}
	pthread_mutex_unlock(&lock);
	*dest = dest_buff;
	last = *dest;
}
