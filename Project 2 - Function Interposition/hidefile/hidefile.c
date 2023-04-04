#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <dlfcn.h>
#include <string.h>

// Name: Michael Nelli
// netID: mrn73
// RUID: 185003528

/*
 * Splits up the environment string by : character and checks if the file name matches
 * any of the split strings.
 */
int match_found(char *env, char *f_name) {
	if (!env || !f_name)
		return 0;
	char *env_copy = (char *) malloc((strlen(env) + 1) * sizeof(char));
	char *tmp = env_copy;
	if (!env_copy)
		return 0;
	strcpy(env_copy, env);
	char *var;
	int match = 0;
	while (var = strsep(&env_copy, ":")) {
		if (*var && strcmp(f_name, var) == 0) {
			match = 1;
			break;
		}
	}
	free(tmp);
	return match;
}

struct dirent *readdir(DIR *dirp) {
	struct dirent *(*real_readdir)(DIR *);
	real_readdir = dlsym(RTLD_NEXT, "readdir");
	struct dirent *ret;
	/* continue skipping over dir entries until we find a file not marked as HIDDEN */
	while ((ret = (*real_readdir)(dirp))) {
		if (!match_found(getenv("HIDDEN"), ret->d_name))
			break;
	}
	return ret;
}
