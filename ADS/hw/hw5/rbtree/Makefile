# 编译器
CC = g++
# 编译选项
CFLAGS = -Wall -Wextra

# 目标文件
TARGET = main

# 源文件
SRCS = main.cpp rbtree.cpp

# 目标文件
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

