
#all: clean compile_gdb
all: compile_gdb

compile:
	gcc -O3 spice.c -o spice

compile_gdb:
	gcc -g -Wall -I/usr/local/include -L/usr/local/lib spice.c MNA_constructor.c parser.c hash_table.c -o spice -lgsl -lgslcblas -lm

clean:
	@rm spice

	
	