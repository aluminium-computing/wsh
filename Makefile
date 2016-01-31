all: clean wsh 
wsh:
	@clang ash.c history.c -lncurses -o out/wsh
clean:
	@rm -rf out/ && mkdir -p out/
<<<<<<< HEAD
install: 
	@mv out/wsh /bin/wsh
=======
install: @mv out/wsh /bin/wsh
>>>>>>> 179f214532c0c08f3328b6005ec85c23c7f0b138
