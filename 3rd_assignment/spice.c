#include "spice.h"

struct node * hash_table[HASH_TABLE_SIZE];

gsl_matrix *mna;
gsl_vector *b;
gsl_vector *x;
gsl_vector *x_help;

int main(int argc, char *argv[]){
	int fd;
	char file_name[50];
	int i, check;	

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
	printf("Parsing the %s\n",file_name);
	parser(&head, fd);
	
	printList(head);
	// make mna
	printf("Constructs the MNA matrix and b vector\n");
	constructor(nodes(),m2_elem(), head);
	// print_MNA(nodes(),m2_elem());
	if(if_sparse){
		sparse_matrix(nodes(), m2_elem());
		
	}
	else{
		if(if_cholesky){
			printf("Computing x with Cholesky analysis \n");
			check = Cholesky_analysis(nodes(),m2_elem());
			if(check == -1){
				return -1;
			}
		}
		else if(if_CG){
			printf("Computing x with CG analysis \n");
			check = CG_analysis(nodes(),m2_elem());
			if(check == -1){
				return -1;
			}
		}
		else if(if_Bi_CG){
			printf("Computing x with Bi_CG analysis \n");
			check = Bi_CG_analysis(nodes(),m2_elem());
			if(check == -1){
				printf("Mulfunction with Bi_CG analysis \n");
				return -1;
			}
		}
		else{
			printf("Computing x with LU analysis \n");
		 	LU_analysis(nodes(),m2_elem());
		}
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
	int new_file = 0;
	char file_name[20];

	
	strcpy(file_name, "DC op_point.txt");

	while(1){
		fd_dc = open(file_name, O_RDONLY, 0777);
		if(fd_dc>0){
			new_file++;
			for(i = 11; i < strlen(file_name); i++){
				file_name[i] = '\0';
			}
			strcat(file_name,"(");
			sprintf(curr_write,"%d", new_file);
			strcat(file_name,curr_write);
			strcat(file_name,").txt");
			close(fd_dc);
		}
		else if(fd_dc<0){
			fd_dc = open(file_name, O_CREAT | O_WRONLY, 0777);
			if(fd_dc < 0){
				perror("open");
				return;
			}
			break;
		}
	}
	if(if_cholesky){
		check = write(fd_dc, "Cholesky_analysis\n", strlen("Cholesky_analysis\n"));
	}
	else if(if_CG){
		check = write(fd_dc, "CG_analysis\n", strlen("CG_analysis\n"));
	}
	else if(if_Bi_CG){
		check = write(fd_dc, "Bi_CG_analysis\n", strlen("Bi_CG_analysis\n"));
	}
	else{
		check = write(fd_dc, "LU_analysis\n", strlen("LU_analysis\n"));
	}
	if(check<0){
		perror("write");
		return;
	}
	check = write(fd_dc, "\nV(0) = 0.000000 Volt\n\n", strlen("\nV(0) = 0.000000 Volt\n\n"));
	if(check<0){
		perror("write");
		return;
	}

	for(i = 0; i< nodes() + m2_elem() - 1; i++){
		check = write(fd_dc, "x(", strlen("V("));
		if(check<0){
			perror("write");
			return;
		}
		sprintf(curr_write,"%d", i+1);
		check = write(fd_dc, curr_write, strlen(curr_write));
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