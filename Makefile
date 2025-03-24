CFLAGS=-Wall -Wextra -std=c11 -pedantic
LIB = src/tx.c src/table.c src/disk.c src/recovery.c src/snapshot.c
SRC_PATH = src

main: src/main.c src/tx.c src/table.c src/disk.c src/recovery.c src/snapshot.c
	$(CC) $(CFLAGS) $(LIB) -g -o main src/main.c
