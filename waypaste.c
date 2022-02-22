#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <unistd.h>

#include "protocol/wlr-data-control-unstable-v1-client-protocol.h"
#include "util.h"

struct zwlr_data_control_manager_v1 *data_control_manager;
struct wl_seat *seat;
struct wl_display *display;

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
offer_offer(void *data, struct zwlr_data_control_offer_v1 *offer, const char *mime_type)
{
	int pipes[2];
	if (strcmp(mime_type, "text/plain") == 0) {
		if (pipe(pipes) == -1)
			die("failed to create pipe");

		zwlr_data_control_offer_v1_receive(offer, mime_type, pipes[1]);
		wl_display_roundtrip(display);
	} else {
		return;
	}

	copyfd(STDOUT_FILENO, pipes[0]);
	close(pipes[0]);

	exit(0);
}

static const struct zwlr_data_control_offer_v1_listener offer_listener = {
	 .offer = offer_offer,
};

void
control_data_offer(void *data, struct zwlr_data_control_device_v1 *device, struct zwlr_data_control_offer_v1 *offer)
{
	zwlr_data_control_offer_v1_add_listener(offer, &offer_listener, NULL);
}

void
control_data_selection(void *data, struct zwlr_data_control_device_v1 *device, struct zwlr_data_control_offer_v1 *offer)
{
}

static const struct zwlr_data_control_device_v1_listener device_listener = {
	.data_offer = control_data_offer,
	.selection = control_data_selection,
};

int
main()
{
	display = wl_display_connect(NULL);
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

	zwlr_data_control_device_v1_add_listener(device, &device_listener, NULL);

	wl_display_roundtrip(display);
}
