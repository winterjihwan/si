CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic -ggdb

main: main.c
	$(CC) $(CFLAGS) -o main main.c

esai: esai.c
	$(CC) $(CFLAGS) -o esai esai.c
