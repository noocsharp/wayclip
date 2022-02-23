PREFIX = /usr/local
LIB = -lwayland-client
WAYCOPY_OBJ = protocol/wlr-data-control-unstable-v1.o waycopy.o common.o util.o
WAYPASTE_OBJ = protocol/wlr-data-control-unstable-v1.o waypaste.o common.o util.o

all: waycopy waypaste

waypaste: protocol/wlr-data-control-unstable-v1-client-protocol.h $(WAYPASTE_OBJ)
	$(CC) $(WAYPASTE_OBJ) $(LIB) -o $@

waycopy: protocol/wlr-data-control-unstable-v1-client-protocol.h $(WAYCOPY_OBJ)
	$(CC) $(WAYCOPY_OBJ) $(LIB) -o $@

protocol/wlr-data-control-unstable-v1.c:
	wayland-scanner private-code protocol/wlr-data-control-unstable-v1.xml protocol/wlr-data-control-unstable-v1.c

protocol/wlr-data-control-unstable-v1-client-protocol.h:
	wayland-scanner client-header protocol/wlr-data-control-unstable-v1.xml protocol/wlr-data-control-unstable-v1-client-protocol.h

.c.o:
	$(CC) -Wall -Wpedantic -c $< -o $@

install:
	install -Dm755 -t $(DESTDIR)$(PREFIX)/bin waycopy waypaste

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/waycopy $(DESTDIR)$(PREFIX)/bin/waypaste

clean:
	rm -f *.o waycopy protocol/*.[och]
