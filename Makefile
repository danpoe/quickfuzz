
NOWARN = -Wno-unused-variable -Wno-unused-function
CFLAGS = -Wall -pedantic

all:
	$(CC) $(CFLAGS) $(NOWARN) -c hash_table.c
	$(CC) $(CFLAGS) $(NOWARN) -c hash_set.c
	$(CC) $(CFLAGS) $(NOWARN) -c quickfuzz.c
	$(CC) $(CFLAGS) $(NOWARN) -DQUICKFUZZ -o test test.c quickfuzz.o hash_table.o hash_set.o -pthread

run:
	./test

clean:
	rm -f test *.o

