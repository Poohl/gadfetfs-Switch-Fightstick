
#ifndef COMMON_H
#define COMMON_H

/*
 * Collection of unspecific and reused stuff for convinience
 */

#include <time.h>
#include <stddef.h>
#include <errno.h>
#include <stdbool.h>

typedef unsigned int uint;

/*
 * Assert Macros for error handling using labels.
 */

#define ASSERT(EXPR, COND, LABEL)  \
	{ \
		typeof(EXPR) ret = (EXPR); \
		if (!(ret COND)) { \
			fprintf(stderr, ("Error: " #EXPR " was %d not " #COND "; errno = %s\n"), ret, strerror(errno)); \
			goto LABEL; \
		} \
	}

#define ASSERTZ(EXPR, LABEL) \
	ASSERT(EXPR, == 0, LABEL)

#define ASSERTNZ(EXPR, LABEL) \
	ASSERT(EXPR, != 0, LABEL)

#define ASSERTT(EXPR, LABEL) \
	ASSERT(EXPR, == true, LABEL)

#define ASSERTF(EXPR, LABEL) \
	ASSERT(EXPR, == false, LABEL)

#define ASSERTNN(EXPR, LABEL) \
	ASSERT(EXPR, >= 0, LABEL)

#define ASSERTP(EXPR, LABEL) \
	ASSERT(EXPR, > 0, LABEL)

#define INFO printf
#define WARNING printf


/*
 * Some utility functions for convinience & debugging
 */

int clamp(int a, int min, int max);

#define setMaskTo(dest, mask, val) \
	(dest & ~mask) | ((val ? 0xFFFF : 0) & mask)

#define mathMod(x,n) \
	(((x) % (n) + (n)) % n)

#define IS_SET(FLAG, VAR) \
	(((FLAG) & (VAR)) ? '0' : '1')

int mapRange(int x, int sl, int sr, int tl, int tr);

void printPoll(int events);

void printMem(const void* src, size_t len);

void timespec_diff(const struct timespec *start, const struct timespec *stop, struct timespec *result);

void timespec_add(const struct timespec *a, const struct timespec *b, struct timespec *result);

// returns the number of characters matching in a and b
uint prefixStrcmp(const char* a, const char* b);

int prefixStrSelect(const char*const * ops, const char* a);

#endif
