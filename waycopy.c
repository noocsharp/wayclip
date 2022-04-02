#define _POSIX_C_SOURCE 1

#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wayland-client.h>
#include <unistd.h>

#include "protocol/wlr-data-control-unstable-v1-client-protocol.h"
#include "common.h"
#include "util.h"

struct wl_registry *registry;
int temp;

bool running = true;

void
data_source_send(void *data, struct zwlr_data_control_source_v1 *source, const char *mime_type, int32_t fd)
{
	lseek(temp, SEEK_SET, 0);

	copyfd(fd, temp);
	close(fd);
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
main(int argc, char *argv[])
{
	close(STDOUT_FILENO);
	argv0 = argv[0];

	options.type = "text/plain";
	options.seat = NULL;
	parseopts("fs:t:", argc, argv);

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
	temp = mkstemp(path);
	if (temp == -1)
		die("failed to create temporary file for copy buffer");

	if (unlink(path) == -1)
		die("failed to remove temporary file");
	copyfd(temp, STDIN_FILENO);
	close(STDIN_FILENO);

	struct wl_display *const display = wl_display_connect(NULL);
	if (display == NULL)
		die("failed to connect to display");

	registry = wl_display_get_registry(display);
	if (registry == NULL)
		die("failed to get registry");

	wl_registry_add_listener(registry, &registry_listener, NULL);

	wl_display_roundtrip(display);
	if (options.seat)
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

	zwlr_data_control_source_v1_offer(source, options.type);
	zwlr_data_control_source_v1_add_listener(source, &data_source_listener, NULL);
	zwlr_data_control_device_v1_set_selection(device, source);

	if (!options.foreground) {
		pid_t pid = fork();
		if (pid == -1) {
			die("failed to fork");
		}

		if (pid != 0) {
			close(STDERR_FILENO);
			return 0;
		}
	}

	while (wl_display_dispatch(display) != -1 && running);

	return running;
}
