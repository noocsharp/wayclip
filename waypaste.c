#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <unistd.h>

#include "protocol/wlr-data-control-unstable-v1-client-protocol.h"
#include "common.h"

struct wl_display *display;
struct zwlr_data_control_offer_v1 *acceptedoffer = NULL;
int pipes[2];

static void
receive(int cond, struct zwlr_data_control_offer_v1 *offer)
{
	if (cond && acceptedoffer == offer) {
		zwlr_data_control_offer_v1_receive(offer, options.type, pipes[1]);
		wl_display_roundtrip(display);
		close(pipes[1]);

		copyfd(STDOUT_FILENO, pipes[0]);
		close(pipes[0]);

		exit(0);
	}

	if (acceptedoffer)
		zwlr_data_control_offer_v1_destroy(acceptedoffer);

	acceptedoffer = NULL;
}

void
offer_offer(void *data, struct zwlr_data_control_offer_v1 *offer, const char *mime_type)
{
	if (acceptedoffer)
		return;

	if (strcmp(mime_type, options.type) == 0)
		acceptedoffer = offer;
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
	receive(!options.primary, offer);
}

void
control_data_primary_selection(void *data, struct zwlr_data_control_device_v1 *device, struct zwlr_data_control_offer_v1 *offer)
{
	receive(options.primary, offer);
}

static const struct zwlr_data_control_device_v1_listener device_listener = {
	.data_offer = control_data_offer,
	.selection = control_data_selection,
	.primary_selection = control_data_primary_selection,
};

int
main(int argc, char *argv[])
{
	argv0 = argv[0];
	parseopts("hps:t:", argc, argv);

	display = wl_display_connect(NULL);
	if (display == NULL)
		die("failed to connect to display");

	struct wl_registry *const registry = wl_display_get_registry(display);
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

	if (pipe(pipes) == -1)
		die("failed to create pipe");

	struct zwlr_data_control_device_v1 *device = zwlr_data_control_manager_v1_get_data_device(data_control_manager, seat);
	if (device == NULL)
		die("data device is null");

	zwlr_data_control_device_v1_add_listener(device, &device_listener, NULL);

	wl_display_roundtrip(display);
}
