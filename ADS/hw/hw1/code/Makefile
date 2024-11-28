CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

.PHONY: all clean
MAIN = test-main
all: $(MAIN)

skiplist.o: skiplist.cc skiplist.h
test-main.o: test-main.cc skiplist.h

$(MAIN): skiplist.o test-main.o
	$(CXX) -o $@ $^

clean:
	rm -f *.o $(MAIN)
