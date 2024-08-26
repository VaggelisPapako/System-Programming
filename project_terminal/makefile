CC = gcc
CFLAGS = -Wall -Werror

SRCS = main.c functions.c aliases.c history.c
OBJS = $(SRCS:.c=.o)
HEADERS = functions.h aliases.h history.c

my_program: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o my_program

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) my_program

run: my_program
	./my_program