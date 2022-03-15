extern bool seat_found;
extern struct wl_seat *seat;
extern struct zwlr_data_control_manager_v1 *data_control_manager;
extern const struct wl_seat_listener seat_listener;
extern const struct wl_registry_listener registry_listener;

extern const char *argv0;

extern struct {
	const char *type;
	const char *seat;
	bool foreground;
} options;
