CFLAGS=-Wall -Wextra -Werror -Wmissing-field-initializers -std=c11 -pedantic -ggdb

main: main.c
	$(CC) $(CFLAGS) -o main main.c

shot: shot.c
	$(CC) $(CFLAGS) -o shot shot.c
