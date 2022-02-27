#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
copyfd(int out, int in)
{
	char buf[BUFSIZ];

	do {
		ssize_t rcount = read(in, buf, BUFSIZ);
		if (rcount == -1) {
			warn("read failed");
			return;
		}

		size_t wcount = 0;
		char *ptr = buf;
		while (wcount < rcount) {
			ssize_t len = write(out, ptr, rcount);
			if (len == -1) {
				warn("read failed");
				return;
			}

			ptr += len;
			wcount += len;
		}
		if (!rcount)
			break;
	} while (1);
}

struct {
	const char *type;
	const char *seat;
} options;

void
parseopts(int argc, char *const argv[])
{
	while (1) {
		int next = getopt(argc, argv, "s:t:");
		if (next == -1) {
			if (argv[optind] && *argv[optind] != '-') {
				fprintf(stderr, "usage: %s [-s seat] [-t mimetype]\n", argv0);
				exit(1);
			}
			break;
		}

		if (next == ':' || next == '?') {
			exit(1);
		}

		switch (next) {
		case 's':
			options.seat = optarg;
			break;
		case 't':
			if (strlen(optarg) > 255) {
				die("mimetype can be at most 255 characters");
			}
			options.type = optarg;
			break;
		}
	}
}
