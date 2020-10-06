
#include "common.h"

#include <poll.h>
#include <stdio.h>

int clamp(int a, int min, int max) {
	return (a > max) ? max : (a < min ? min : a);
}

int mapRange(int x, int sl, int sr, int tl, int tr) {
	return (x - sl) * (tr - tl) / (sr - sl) + tl;
}

void printPoll(int events) {
	printf("TOTAL: %x\n"
			"IN:  %c OUT: %c HUP: %c\n"
			"PRI: %c ERR: %c NVAL: %c\n",
			events,
			IS_SET(POLLIN, events), IS_SET(POLLOUT, events), IS_SET(POLLHUP, events),
			IS_SET(POLLPRI, events), IS_SET(POLLERR, events), IS_SET(POLLNVAL, events)
		);
}

#define hexStr "0123456789ABCDEF"

void printMem(const void* src, size_t len) {
	char* c = (char*) src;
	char* end = c + len;
	for (; c < end; ++c) {
		printf("%c%c", hexStr[*c >> 4], hexStr[*c & 0xF]);
	}
	printf("\n");
}

void timespec_diff(const struct timespec *start, const struct timespec *stop, struct timespec *result) {
	if ((stop->tv_nsec - start->tv_nsec) < 0) {
		result->tv_sec = stop->tv_sec - start->tv_sec - 1;
		result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
	} else {
		result->tv_sec = stop->tv_sec - start->tv_sec;
		result->tv_nsec = stop->tv_nsec - start->tv_nsec;
	}
	return;
}


void timespec_add(const struct timespec *a, const struct timespec *b, struct timespec *result) {
	result->tv_nsec = a->tv_nsec + b->tv_nsec;
	result->tv_sec = a->tv_sec + b->tv_sec + result->tv_nsec / 1000000000;
	result->tv_nsec %= 1000000000;
}

uint prefixStrcmp(const char* a, const char* b) {
	uint i;
	for (i = 0; a[i] && b[i] && a[i] == b[i]; i++);
	return i;
}

int prefixStrSelect(const char*const * ops, const char* a) {
	uint tmp, max = 0;
	int amax = -1;
	for (uint c = 0; ops[c]; ++c) {
		if ((tmp =prefixStrcmp(ops[c], a)) > max) {
			max = tmp;
			amax = c;
		}
	}
	return amax;
}
