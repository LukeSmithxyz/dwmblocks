.POSIX:

PREFIX = /usr/local
CC = gcc

dwmblocks: dwmblocks.o
	$(CC) dwmblocks.o -lX11 -o dwmblocks
dwmblocks.o: dwmblocks.c config.h
	$(CC) -c dwmblocks.c
clean:
	rm -f *.o *.gch dwmblocks
install: dwmblocks
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f dwmblocks $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dwmblocks
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/dwmblocks

.PHONY: clean install uninstall
