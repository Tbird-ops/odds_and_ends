CC = gcc
CFLAGS = -no-pie

final:
	$(CC) $(CFLAGS) -o final.bin system.secure.c; echo -n "CHANGEME12345678" > system_key.enc

clean:
	rm -f final.bin system_key.enc
