
#all: clean compile_gdb
#gcc -g -Wall -I/usr/local/include -L/usr/local/lib *.c -o spice -lm -L  ./Sparse/CXSparse/Lib -lcxsparse -lgsl -lgslcblas 
#gcc -O3 *.c -o circuit_simulation -lblas -lm -L ./Sparse/CXSparse/Lib -lcxsparse

all: compile_gdb

compile:
	gcc -O3 spice.c -o spice

compile_gdb:
	gcc  -g -Wall -I/usr/local/include -L/usr/local/lib  *.c  -o spice -lblas  -L ./Sparse/CXSparse/Lib  -lm -lcxsparse  -lm -lgsl -lgslcblas 

clean:
	@rm spice

	
	