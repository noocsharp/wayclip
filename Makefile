.c.o:
	$(CC) -Wall -c $< -o $@

waycopy: protocol/wlr-data-control-unstable-v1-client-protocol.h protocol/wlr-data-control-unstable-v1.o waycopy.o util.o
	$(CC) protocol/wlr-data-control-unstable-v1.o waycopy.o util.o -lwayland-client -o waycopy

protocol/wlr-data-control-unstable-v1.c:
	wayland-scanner private-code protocol/wlr-data-control-unstable-v1.xml protocol/wlr-data-control-unstable-v1.c

protocol/wlr-data-control-unstable-v1-client-protocol.h:
	wayland-scanner client-header protocol/wlr-data-control-unstable-v1.xml protocol/wlr-data-control-unstable-v1-client-protocol.h

clean:
	rm -f *.o waycopy protocol/*.[och]
