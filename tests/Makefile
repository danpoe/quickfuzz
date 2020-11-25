.PHONY: clean all

WARN = -Wall -pedantic
NOWARN = -Wno-unused-variable -Wno-unused-function

CXXFLAGS = $(WARN) $(NOWARN)

DEPS = \
  ../hash_table.o \
	../hash_set.o

OBJS = \
  hash_table_test.o \
	hash_set_test.o

all: $(OBJS)
	$(CXX) $(CXXFLAGS) main.cpp $(OBJS) $(DEPS) -o main

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o main