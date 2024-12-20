CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb

tx: tx.c table.c disk.c recovery.c
	$(CC) $(CFLAGS) table.c disk.c recovery.c -o tx tx.c

snapshot: snapshot.c disk.c
	$(CC) $(CFLAGS) table.c disk.c -o snapshot snapshot.c
