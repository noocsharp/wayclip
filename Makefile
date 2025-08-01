.POSIX:
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man
LIB = -lwayland-client
EXE = waycopy waypaste
OBJ_COMMON = protocol/ext-data-control-v1.o common.o

all: $(EXE)

waypaste: waypaste.o $(OBJ_COMMON)
	$(CC) waypaste.o $(OBJ_COMMON) $(LIB) -o $@

waycopy: waycopy.o $(OBJ_COMMON)
	$(CC) waycopy.o $(OBJ_COMMON) $(LIB) -o $@

waycopy.o: waycopy.c common.h protocol/ext-data-control-v1-client-protocol.h

waypaste.o: waypaste.c common.h protocol/ext-data-control-v1-client-protocol.h

protocol/ext-data-control-v1.c: protocol/ext-data-control-v1.xml
	wayland-scanner private-code protocol/ext-data-control-v1.xml $@

protocol/ext-data-control-v1-client-protocol.h: protocol/ext-data-control-v1.xml
	wayland-scanner client-header protocol/ext-data-control-v1.xml $@

.c.o:
	$(CC) -Wall -Wpedantic $(CFLAGS) -c $< -o $@

install:
	install -Dm755 -t $(DESTDIR)$(PREFIX)/bin $(EXE)
	install -Dm644 -t $(DESTDIR)$(MANPREFIX)/man1 waycopy.1 waypaste.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/waycopy $(DESTDIR)$(PREFIX)/bin/waypaste \
	      $(DESTDIR)$(MANPREFIX)/man1/waycopy.1 $(DESTDIR)$(MANPREFIX)/man1/waypaste.1

clean:
	rm -f *.o $(EXE) protocol/*.[och]
