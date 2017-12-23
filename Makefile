
NOWARN = -Wno-unused-variable -Wno-unused-function
CFLAGS = -Wall

all:
	$(CC) $(CFLAGS) $(NOWARN) -c quickfuzz.c
	$(CC) $(CFLAGS) $(NOWARN) -DQUICKFUZZ -o test test.c quickfuzz.o -pthread

run:
	./test

clean:
	rm -f test *.o

