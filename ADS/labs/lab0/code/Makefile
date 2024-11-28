cc = g++
std = c++20
prog = main
objects = main.o hftree.o

$(prog): $(objects)
	@$(cc) -std=$(std) -g -o $(prog) $(objects)

main.o : hftree.h utils.h
	@$(cc) -c -g -o main.o main.cpp

hftree.o: hftree.h
	@$(cc) -c -g -o hftree.o hftree.cpp	

.PHONY: grade
grade:
	@make -s clean
	@make -s
ifeq ($(OS), Windows_NT)
	@powershell.exe -ExecutionPolicy Bypass -File .\grade.ps1
else
	@./grade.sh
endif
	@make -s clean

.PHONY:clean
clean:
ifeq ($(OS), Windows_NT)
	@del /q .\*.o .\output\* 2>nul
	@del /q .\$(prog).exe 2>nul
else
	@rm -rf ./*.o ./output/* $(prog)
endif