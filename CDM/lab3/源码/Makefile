.PHONY: method1 method2 method3 handin

method1:
	./dafny/dafny method1.dfy 
	@rm method1.dll method1.runtimeconfig.json > /dev/null 2>&1

method2:
	./dafny/dafny method2.dfy 
	@rm method2.dll method2.runtimeconfig.json  > /dev/null 2>&1

method3:
	./dafny/dafny method3.dfy 
	@rm method3.dll method3.runtimeconfig.json  > /dev/null 2>&1

handin:
	zip lab3.zip *.dfy
	@echo “Please upload lab3.zip to canvas.”