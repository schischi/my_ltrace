CC=clang
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99 -g3
LDFLAGS=-lelf

-include src/my_strace/strace.mk
-include src/my_ltrace/ltrace.mk

LTRACE_CFILES=$(addprefix src/my_ltrace/, $(LTRACE_FILES))
LTRACE_OBJS=$(LTRACE_CFILES:.c=.o)

my_ltrace: $(LTRACE_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(LTRACE_OBJS)
	rm -f my_ltrace
