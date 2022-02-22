#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wayland-client.h>
#include <unistd.h>

#include "protocol/wlr-data-control-unstable-v1-client-protocol.h"
#include "util.h"

extern const char *argv0;

#define MIMETYPE_MAX_SIZE 256
char mimetype[MIMETYPE_MAX_SIZE];

struct zwlr_data_control_manager_v1 *data_control_manager;
struct wl_seat *seat;
FILE *temp;

bool running = 1;

void
registry_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
	if (strcmp(interface, "wl_seat") == 0) {
		seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
	} else if (strcmp(interface, "zwlr_data_control_manager_v1") == 0) {
		data_control_manager = wl_registry_bind(registry, name, &zwlr_data_control_manager_v1_interface, 1);
	}
}

void
registry_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
	.global = registry_global,
	.global_remove = registry_global_remove,
};

void
data_source_send(void *data, struct zwlr_data_control_source_v1 *source, const char *mime_type, int32_t fd)
{
	fseek(temp, SEEK_SET, 0);
	FILE *out = fdopen(fd, "w");
	if (out == NULL) {
		warn("failed to open fd as FILE");
	}

	copyfile(out, temp);
	fclose(out);
}

void
data_source_cancelled(void *data, struct zwlr_data_control_source_v1 *source)
{
	running = 0;
}

static const struct zwlr_data_control_source_v1_listener data_source_listener = {
	.send = data_source_send,
	.cancelled = data_source_cancelled,
};

const char *const tempname = "/waycopy-buffer-XXXXXX";

int
main(int argc, const char *argv[])
{
	argv0 = argv[0];

	char path[PATH_MAX] = {0};
	char *ptr = getenv("TMPDIR");
	if (ptr == NULL)
		strcpy(path, "/tmp");
	else {
		if (strlen(ptr) > PATH_MAX - strlen(tempname))
			die("TMPDIR has too long of a path");

		strcpy(path, ptr);
	}

	strncat(path, tempname, PATH_MAX - 1);
	int tempfd = mkstemp(path);
	if (tempfd == -1)
		die("failed to create temporary file for copy buffer");

	temp = fdopen(tempfd, "r+");
	if (temp == NULL) {
		die("failed to open temporary file as FILE");
	}

	copyfile(temp, stdin);
	fclose(stdin);

	struct wl_display *const display = wl_display_connect(NULL);
	if (display == NULL)
		die("failed to connect to display");

	struct wl_registry *const registry = wl_display_get_registry(display);
	if (registry == NULL)
		die("failed to get registry");

	wl_registry_add_listener(registry, &registry_listener, NULL);

	wl_display_roundtrip(display);

	if (seat == NULL)
		die("failed to bind to seat interface");

	if (data_control_manager == NULL)
		die("failed to bind to data_control_manager interface");

	struct zwlr_data_control_device_v1 *device = zwlr_data_control_manager_v1_get_data_device(data_control_manager, seat);
	if (device == NULL)
		die("data device is null");

	struct zwlr_data_control_source_v1 *source = zwlr_data_control_manager_v1_create_data_source(data_control_manager);
	if (source == NULL)
		die("source is null");

	zwlr_data_control_source_v1_offer(source, "text/plain");
	zwlr_data_control_source_v1_add_listener(source, &data_source_listener, NULL);
	zwlr_data_control_device_v1_set_selection(device, source);

	while (wl_display_dispatch(display) != -1 && running);

	unlink(path);
	return running;
}
