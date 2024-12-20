CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb

main: main.c
	$(CC) $(CFLAGS) -o main main.c

tx: tx.c table.c disk.c recovery.c
	$(CC) -g $(CFLAGS) table.c disk.c recovery.c -o tx tx.c

disk: disk.c disk.h
	$(CC) $(CFLAGS) table.c -o disk disk.c

table: table.c table.h
	$(CC) $(CFLAGS) -o table table.c
