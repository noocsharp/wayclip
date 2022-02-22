#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "util.h"

const char *argv0;

void
die(const char *const error)
{
	fprintf(stderr, "%s: %s\n", argv0, error);
	exit(1);
}

void
warn(const char *const error)
{
	fprintf(stderr, "%s: warning: %s\n", argv0, error);
}

void
copyfile(FILE *out, FILE *in)
{
	char buf[BUFSIZ];

	size_t rcount, wcount;
	do {
		rcount = fread(buf, 1, BUFSIZ, in);
		wcount = fwrite(buf, 1, rcount, out);
		if (rcount < BUFSIZ) {
			if (ferror(in)) {
				// TODO: print actual error
				warn("fread failed");
			} else if (feof(in)) {
				break;
			}
		}

		if (wcount < rcount) {
			warn("fwrite failed");
		}
	} while (1);
}

void
copyfd(int out, int in)
{
	char buf[BUFSIZ], *ptr;

	size_t rcount, wcount;
	do {
		rcount = read(in, buf, BUFSIZ);
		if (rcount == -1) {
			warn("read failed");
		}

		wcount = 0;
		ptr = buf;
		while (wcount < rcount) {
			ssize_t len = write(out, ptr, rcount);
			if (len == -1) {
				warn("read failed");
				return;
			}

			ptr += len;
			wcount += len;
		}
		if (rcount < BUFSIZ)
			break;
	} while (1);
}
