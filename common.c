#define _POSIX_C_SOURCE 2
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wayland-client.h>

#include "protocol/wlr-data-control-unstable-v1-client-protocol.h"
#include "common.h"

const char *argv0;
static bool seat_found = false;

struct wl_seat *seat;
struct zwlr_data_control_manager_v1 *data_control_manager;

struct options options = {
	.type = "text/plain;charset=utf-8"
};

static void
seat_capabilities(void *data, struct wl_seat *seat, uint32_t cap)
{
}

static void
seat_name(void *data, struct wl_seat *_seat, const char *name)
{
	if (!seat_found && strcmp(name, options.seat) == 0) {
		seat_found = true;
		seat = _seat;
	} else wl_seat_destroy(_seat);
}

static const struct wl_seat_listener seat_listener = {
	.capabilities = seat_capabilities,
	.name = seat_name,
};

void
registry_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
	if (!seat_found && strcmp(interface, "wl_seat") == 0) {
		seat = wl_registry_bind(registry, name, &wl_seat_interface, 2);
		if (options.seat) {
			wl_seat_add_listener(seat, &seat_listener, NULL);
			seat = NULL;
		} else seat_found = true;
	} else if (strcmp(interface, "zwlr_data_control_manager_v1") == 0) {
		data_control_manager = wl_registry_bind(registry, name, &zwlr_data_control_manager_v1_interface, 1);
	}
}

void
registry_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
}

const struct wl_registry_listener registry_listener = {
	.global = registry_global,
	.global_remove = registry_global_remove,
};

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
copyfd(const int out, const int in)
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

static void
usage(int code)
{
	fprintf(stderr, "usage: %s [-s seat] [-t mimetype]\n", argv0);
	exit(code);
}

void
parseopts(const char *opts, int argc, char *const argv[])
{
	while (1) {
		int next = getopt(argc, argv, opts);
		if (next == -1) {
			if (argv[optind] && *argv[optind] != '-')
				usage(1);

			break;
		}

		if (next == ':' || next == '?')
			exit(1);

		switch (next) {
		case 'f':
			options.foreground = true;
			break;
		case 'h':
			usage(0);
		case 's':
			options.seat = optarg;
			break;
		case 't':
			if (strlen(optarg) > 255)
				die("mimetype can be at most 255 characters");

			options.type = optarg;
			break;
		}
	}
}
