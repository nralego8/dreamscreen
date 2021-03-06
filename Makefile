CC=gcc

default: dreamscreend
debug: CFLAGS += -DDEBUG
debug: dreamscreend

dreamscreend.o: dreamscreend.c
	$(CC) -c dreamscreend.c -o dreamscreend.o

dreamscreend: dreamscreend.o
	$(CC) dreamscreend.o -o dreamscreend -lcurl -lm

clean:
	-rm -f dreamscreend.o
	-rm -f dreamscreend

install:
	-install -m 755 dreamscreend /usr/local/bin/
	-install -m 744 dreamscreen.service /usr/lib/systemd/system/
	-systemctl daemon-reload

uninstall:
	-systemctl stop dreamscreen
	-systemctl disable dreamscreen
	-rm -f /usr/local/bin/dreamscreend
	-rm -f /usr/lib/systemd/system/dreamscreen.service
	-systemctl daemon-reload
