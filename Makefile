all: clean wsh 
wsh:
	@clang ash.c history.c -o out/wsh
clean:
	@rm -rf out/ && mkdir -p out/
install: @mv out/wsh /bin.wsh
