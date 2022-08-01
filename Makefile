.POSIX:
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man
LIB = -lwayland-client
WAYCOPY_OBJ = protocol/wlr-data-control-unstable-v1.o waycopy.o common.o
WAYPASTE_OBJ = protocol/wlr-data-control-unstable-v1.o waypaste.o common.o
EXE = waycopy waypaste

all: $(EXE)

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
	install -Dm644 -t $(DESTDIR)$(MANPREFIX)/man1 waycopy.1 waypaste.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/waycopy $(DESTDIR)$(PREFIX)/bin/waypaste \
	      $(DESTDIR)$(MANPREFIX)/man1/waycopy.1 $(DESTDIR)$(MANPREFIX)/man1/waypaste.1

clean:
	rm -f *.o $(EXE) protocol/*.[och]
