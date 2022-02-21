#include <stdlib.h>
#include <stdio.h>

#include "util.h"

const char *argv0;

void
die(const char *const error)
{
	fprintf(stderr, "%s: %s\n", argv0, error);
	exit(1);
}

