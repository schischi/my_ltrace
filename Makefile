CC=clang
CFLAGS=-Wall -Wextra -Werror -std=c99 -I.
LDFLAGS=-lelf

-include src/my_strace/strace.mk
STRACE_CFILES=$(addprefix src/my_strace/, $(STRACE_FILES))
STRACE_OBJS=$(STRACE_CFILES:.c=.o)

-include src/my_ltrace/ltrace.mk
LTRACE_CFILES=$(addprefix src/my_ltrace/, $(LTRACE_FILES))
LTRACE_OBJS=$(LTRACE_CFILES:.c=.o)

all: my_ltrace my_strace

my_ltrace: $(LTRACE_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

my_strace: $(STRACE_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test:
	$(CC) tests/main.c -o tests/simple
	$(CC) tests/fork.c -o tests/fork

clean:
	rm -f $(LTRACE_OBJS)
	rm -f $(STRACE_OBJS)
	rm -f my_ltrace
	rm -f my_strace
	rm -f tests/fork
	rm -f tests/simple

distclean:
	rm -rf syscalls.def
