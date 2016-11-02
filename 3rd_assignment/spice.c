#include "spice.h"

struct node * hash_table[HASH_TABLE_SIZE];

gsl_matrix *mna;
gsl_vector *b;
gsl_vector *x;
gsl_vector *x_help;

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
		scanf(" %s",file_name );
		printf("The file that you gave me is \"%s\"\n",file_name );
	}
	else {
		strcpy(file_name, argv[1]);
	}

	
	//We open the file where the circuit is.
	fd = open(file_name, O_RDONLY);
	if(fd<0){
		printf("Error at the opening of the file.\n");
		return(-1);
	}

	struct element *head;
	head = NULL;

	//parsing the elements
	parser(&head, fd);
	
	// printList(head);
	//make mna
	constructor(nodes(),m2_elem(), head);
	// print_MNA(nodes(),m2_elem());
	if(if_cholesky == 0){
	 	LU_analysis(nodes(),m2_elem());
	} else {
		Cholesky_analysis(nodes(),m2_elem());
	}

	print_x();
	
	//handles PRINT|PLOT if exist in netlist
	plot(head);

	//free the structurs
	free_mna();
	free_elements(&head);
	free_nodes(hash_table);
		
	//close file
	close(fd);
	return(0);
}

void print_x(){
	int fd_dc, check, i;
	char curr_write[20];

	fd_dc = open("DC op_point.txt", O_CREAT | O_WRONLY, 0777);
	if(fd_dc<0){
		printf("Error at the opening of the file.\n");
		return;
	}
	if(if_cholesky == 1){
		check = write(fd_dc, "Cholesky_analysis\n", strlen("Cholesky_analysis\n"));
	}
	else{
		check = write(fd_dc, "LU_analysis\n", strlen("LU_analysis\n"));
	}
	if(check<0){
		perror("write");
		return;
	}
	check = write(fd_dc, "\nV(0) = 0.000000\n", strlen("\nV(0) = 0.000000\n"));
	if(check<0){
		perror("write");
		return;
	}

	for(i = 0; i< nodes() - 1; i++){
		check = write(fd_dc, "V(", strlen("V("));
		if(check<0){
			perror("write");
			return;
		}
		sprintf(curr_write,"%s", find_value(gsl_vector_get(x_help,i)));
		check = write(fd_dc, find_value(gsl_vector_get(x_help,i)), strlen(find_value(gsl_vector_get(x_help,i))));
		if(check<0){
			perror("write");
			return;
		}
		check = write(fd_dc, ") = ", strlen(") = "));
		if(check<0){
			perror("write");
			return;
		}
		sprintf(curr_write,"%lf", gsl_vector_get(x,i));
		check = write(fd_dc, curr_write, strlen(curr_write));
		if(check<0){
			perror("write");
			return;
		}
		check = write(fd_dc, " Volt\n", strlen(" Volt\n"));
		if(check<0){
			perror("write");
			return;
		}
	}

	close(fd_dc);
}