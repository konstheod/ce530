#include <stdio.h>
#include <fcntl.h> // for open read write
#include <unistd.h>
#include <ctype.h> //for toupper, tolower, isspace
#include <string.h>
#include <stdlib.h> // for malloc free 


#include "hash_table.h"
#include "parser.h"
#include "MNA_constructor.h"

struct node * hash_table[HASH_TABLE_SIZE];

double **mna;
double *x, *b;

struct node * hash_table[HASH_TABLE_SIZE];

int main(int argc, char *argv[]){
	int fd;
	char file_name[50];
	
<<<<<<< HEAD
	int i;	
=======
	int i;
>>>>>>> 8a849c82e8f896df5b48a4210af1c5bc9c2df87b
	
	for(i = 0; i < HASH_TABLE_SIZE; i++){
	    
	    hash_table[i] = NULL;
	}

	//We ask for the file name of the circuit.
	strcpy(file_name, argv[1]);
<<<<<<< HEAD
=======
	
	//strcpy(file_name, "circuit1.netlist");
	//strcpy(file_name, "circuit1_abnormal.netlist");
	//strcpy(file_name, "transistor_circuit.netlist");
	//strcpy(file_name, "transistor_circuit_abnormal.netlist");
>>>>>>> 8a849c82e8f896df5b48a4210af1c5bc9c2df87b

	printf("The file that you gave me is \"%s\"\n",file_name );
	
	//We open the file where the circuit is.
	fd = open(file_name, O_RDONLY);
	if(fd<0){
		printf("Error at the opening of the file.\n");
		return(-1);
	}

	struct element *head;
	head = NULL;

	parser(&head, fd);
	
	printList(head);
<<<<<<< HEAD
	constructor(nodes(),m2_elem(), head);
	print_MNA(nodes(),m2_elem());
	
	
	free_mna(nodes(),m2_elem());
=======
	
>>>>>>> 8a849c82e8f896df5b48a4210af1c5bc9c2df87b
	free_elements(&head);
	free_nodes(hash_table);

	close(fd);
	return(0);
}