#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <time.h>

// Name: Michael Nelli
// netID: mrn73
// RUID: 185003528

int is_first_call = 1;
time_t time(time_t *tloc) {
	if (is_first_call) {
		is_first_call = 0;
		return (*tloc = 1609549200);
	}
	time_t (*real_time)(time_t *);
	real_time = dlsym(RTLD_NEXT, "time");
	return (*real_time)(tloc);
}
