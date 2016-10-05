#include <stdio.h>
#include <fcntl.h> // for open read write
#include <unistd.h>
#include <ctype.h> //for toupper, tolower, isspace
#include <string.h>
#include <stdlib.h> // for malloc free 


#include "hash_table.h"
#include "parser.h"

int main(int argc, char *argv[]){
	int fd;
	char file_name[50];

	//We ask for the file name of the circuit.
	printf("Give me the exact file name: ");
	scanf("%s",file_name );
	
	//strcpy(file_name, "circuit1.netlist");
	//strcpy(file_name, "circuit1_abnormal.netlist");
	//strcpy(file_name, "transistor_circuit.netlist");
	//strcpy(file_name, "transistor_circuit_abnormal.netlist");

	printf("The file that you gave me is \"%s\"\n",file_name );
	
	//We open the file where the circuit is.
	fd = open(file_name, O_RDONLY);
	if(fd<0){
		printf("Error at the opening of the file.\n");
		return(-1);
	}

	struct element *head;
	struct node *hash_head;
	hash_head = NULL;
	head = NULL;

	parser(&head, &hash_head, fd);

	printList(head, hash_head);
	free_elements(&head);
	free_nodes(&hash_head);

	close(fd);
	return(0);
}