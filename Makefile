CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb

main: main.c
	$(CC) $(CFLAGS) -o main main.c

esai: esai.c
	$(CC) $(CFLAGS) -o esai esai.c

disk: disk.c disk.h
	$(CC) $(CFLAGS) table.c -o disk disk.c

table: table.c table.h
	$(CC) $(CFLAGS) -o table table.c
