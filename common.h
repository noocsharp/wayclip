extern bool seat_found;
extern struct wl_seat *seat;
extern struct zwlr_data_control_manager_v1 *data_control_manager;
extern const struct wl_seat_listener seat_listener;
extern const struct wl_registry_listener registry_listener;

extern const char *argv0;

extern struct options {
	const char *type;
	const char *seat;
	bool foreground;
} options;

void die(const char *const error);
void warn(const char *const error);
void copyfd(int out, int in);
void parseopts(const char *opts, int argc, char *const argv[]);
