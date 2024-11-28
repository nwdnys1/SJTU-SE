LINK.o = $(LINK.cc)
CXXFLAGS = -std=c++20 -Wall -g

all: correctness persistence test

correctness: kvstore.o correctness.o memtable.o

persistence: kvstore.o persistence.o memtable.o

test: kvstore.o test.o memtable.o

kvstore.o correctness.o persistence.o test.o memtable.o: config.h

clean:
	-rm -f correctness persistence test *.o
