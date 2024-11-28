CC := g++
STD := c++17
prog := main
test_prog := test
objects := main.o radix_tree.o compressed_radix_tree.o test.o

# 构建所有程序
all: $(prog) $(test_prog)

# 主程序构建规则
$(prog): main.o radix_tree.o compressed_radix_tree.o
	$(CC) -std=$(STD) -g -o $@ $^

# 测试程序构建规则
$(test_prog): test.o radix_tree.o compressed_radix_tree.o
	$(CC) -std=$(STD) -g -o $@ $^

# 通用的目标文件构建规则
%.o: %.cpp
	$(CC) -std=$(STD) -g -c $< -o $@

# 清理目标
clean:
	rm -rf $(objects) $(prog) $(test_prog)
grade: $(prog)
ifeq ($(OS), Windows_NT)
	@powershell.exe -ExecutionPolicy Bypass -File .\grade.ps1
else
	@./grade.sh
endif


