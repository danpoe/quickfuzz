.PHONY: clean all

WARN = -Wall -pedantic -Werror
NOWARN = -Wno-unused-variable -Wno-unused-function

CFLAGS = $(WARN) $(NOWARN)

OBJS = \
  hash_table.o \
	hash_set.o \
	quickfuzz.o

all: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	make -C tests clean
