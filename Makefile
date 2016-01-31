all: clean wsh 
wsh:
	@clang ash.c history.c -lncurses -o out/wsh
clean:
	@rm -rf out/ && mkdir -p out/
install: @mv out/wsh /bin/wsh
