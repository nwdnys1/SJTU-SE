CC = g++
CFLAGS = -std=c++11 -pthread

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = fibonacci

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC) $(OBJS)

