
all: clean compile_gdb


compile:
	gcc -O3 spice.c -o spice

compile_gdb:
	gcc -g -Wall spice.c -o spice

clean:
	@rm spice
