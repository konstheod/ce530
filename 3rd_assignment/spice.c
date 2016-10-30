#include "spice.h"

struct node * hash_table[HASH_TABLE_SIZE];

gsl_matrix *mna;
gsl_vector *b;

int main(int argc, char *argv[]){
	int fd;
	char file_name[50];
	
	int i;	
	for(i = 0; i < HASH_TABLE_SIZE; i++){
	    
	    hash_table[i] = NULL;
	}

	//We ask for the file name of the circuit.
	if(argv[1] == NULL){
		printf("Give me the exact file name: ");
		scanf("%s",file_name );
	}
	else {
		strcpy(file_name, argv[1]);
	}

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
	constructor(nodes(),m2_elem(), head);
	print_MNA(nodes(),m2_elem());
	if(strcmp(argv[2],"1")==0){
		LU_analysis(nodes(),m2_elem());
	} else {
		Cholesky_analysis(nodes(),m2_elem());
	}
	
	free_mna();
	free_elements(&head);
	free_nodes(hash_table);

	close(fd);
	return(0);
}