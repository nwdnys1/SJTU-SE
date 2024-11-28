lab1: main.cpp lab1.cpp common.h
	@g++ $^ -lminisat -o $@
	@./$@

example: minisat.cpp
	@g++ $^ -lminisat -o $@
	@./$@

clean:
	@rm -rf lab1
	@rm -rf example
	@rm -rf answer.txt
	@echo "Clean done"

handin:
	zip lab1.zip lab1.cpp
	@echo “Please upload lab1.zip to canvas.”
