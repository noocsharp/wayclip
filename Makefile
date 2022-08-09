.POSIX:
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man
LIB = -lwayland-client
EXE = waycopy waypaste
OBJ_COMMON = protocol/wlr-data-control-unstable-v1.o common.o

all: $(EXE)

waypaste: waypaste.o $(OBJ_COMMON)
	$(CC) $< $(OBJ_COMMON) $(LIB) -o $@

waycopy: waycopy.o $(OBJ_COMMON)
	$(CC) $< $(OBJ_COMMON) $(LIB) -o $@

waycopy.o: waycopy.c common.h protocol/wlr-data-control-unstable-v1-client-protocol.h
	$(CC) -Wall -Wpedantic -c $< -o $@

waypaste.o: waypaste.c common.h protocol/wlr-data-control-unstable-v1-client-protocol.h
	$(CC) -Wall -Wpedantic -c $< -o $@

protocol/wlr-data-control-unstable-v1.c: protocol/wlr-data-control-unstable-v1.xml
	wayland-scanner private-code $< $@

protocol/wlr-data-control-unstable-v1-client-protocol.h: protocol/wlr-data-control-unstable-v1.xml
	wayland-scanner client-header $< $@

.c.o:
	$(CC) -Wall -Wpedantic -c $< -o $@

install:
	install -Dm755 -t $(DESTDIR)$(PREFIX)/bin $(EXE)
	install -Dm644 -t $(DESTDIR)$(MANPREFIX)/man1 waycopy.1 waypaste.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/waycopy $(DESTDIR)$(PREFIX)/bin/waypaste \
	      $(DESTDIR)$(MANPREFIX)/man1/waycopy.1 $(DESTDIR)$(MANPREFIX)/man1/waypaste.1

clean:
	rm -f *.o $(EXE) protocol/*.[och]
