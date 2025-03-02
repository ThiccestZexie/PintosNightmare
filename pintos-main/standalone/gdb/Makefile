CFLAGS := -Wall -Wextra -std=gnu99 -pedantic -g
TARGETS := debug1 debug2 debug3

all: $(TARGETS)

$(TARGETS): %: %.c memory.c
	gcc $(CFLAGS) -o $@ $^

# Disable built-in rules.
.SUFFIXES:

.PHONY: clean all
clean:
	rm -f $(TARGETS)
