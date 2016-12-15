#include "spice.h"

struct node * hash_table[HASH_TABLE_SIZE];

gsl_matrix *mna;
gsl_matrix *C;
gsl_vector *b;
gsl_vector *x;
int vol_counter;
unsigned long int *x_help;

gsl_matrix *mna_tran;
gsl_matrix *mna_curr;
gsl_matrix *C;
gsl_vector *e;
gsl_vector *e_prev;
gsl_vector *b_curr;

cs_di *C_sparse;
cs_di *MNA_tran_sparse;
cs_di *mna_curr_sparse;
cs_di *compressed_MNA;

double *b_sparse;
double *x_sparse;
double *e_sparse;
double *b_curr_sparse;
double *e_prev_sparse;
int non_zeros;

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
	
	// printList(head);
	
	if(if_sparse){
		printf("Constructs the MNA matrix and b vector\n");
		constructor_sparse(nodes(), m2_elem(), head);
		// print_MNA_sparse(nodes(), m2_elem());
		sparse_matrix(nodes(), m2_elem());
		print_x();	
		plot(head);	
	}
	else{

		// make mna
		printf("Constructs the MNA matrix and b vector\n");
		constructor(nodes(),m2_elem(), head);
		// print_MNA(nodes(),m2_elem());
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
		print_x();
		//handles PRINT|PLOT if exist in netlist
		plot(head);	
	}



	

	//free the structurs
	if(if_sparse) {
		free_mna_sparse();
		cs_di_spfree(compressed_MNA);
	}
	else {
		free_mna();
	}
	 if(if_tran) {
	 	
		tran_free();
	 }
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
	// if(if_cholesky){
	// 	check = write(fd_dc, "Cholesky_analysis\n", strlen("Cholesky_analysis\n"));
	// }
	// else if(if_CG){
	// 	check = write(fd_dc, "CG_analysis\n", strlen("CG_analysis\n"));
	// }
	// else if(if_Bi_CG){
	// 	check = write(fd_dc, "Bi_CG_analysis\n", strlen("Bi_CG_analysis\n"));
	// }
	// else{
	// 	check = write(fd_dc, "LU_analysis\n", strlen("LU_analysis\n"));
	// }
	// if(check<0){
	// 	perror("write");
	// 	return;
	// }
	// check = write(fd_dc, "\nV(0) = 0.000000 Volt\n\n", strlen("\nV(0) = 0.000000 Volt\n\n"));
	// if(check<0){
	// 	perror("write");
	// 	return;
	// }

	// for(i = 0; i< nodes() + m2_elem() - 1; i++){
	// 	check = write(fd_dc, "x(", strlen("V("));
	// 	if(check<0){
	// 		perror("write");
	// 		return;
	// 	}
	// 	sprintf(curr_write,"%d", i+1);
	// 	check = write(fd_dc, curr_write, strlen(curr_write));
	// 	if(check<0){
	// 		perror("write");
	// 		return;
	// 	}
	// 	check = write(fd_dc, ") = ", strlen(") = "));
	// 	if(check<0){
	// 		perror("write");
	// 		return;
	// 	}
	// 	sprintf(curr_write,"%lf", gsl_vector_get(x,i));
	// 	check = write(fd_dc, curr_write, strlen(curr_write));
	// 	if(check<0){
	// 		perror("write");
	// 		return;
	// 	}
	// 	check = write(fd_dc, " Volt\n", strlen(" Volt\n"));
	// 	if(check<0){
	// 		perror("write");
	// 		return;
	// 	}
	// }

	for(i = 0; i< (nodes() - 1); i++){
 		// check = write(fd_dc, "V(", strlen("V("));
 		// if(check<0){
 		// 	perror("write");
 		// 	return;
 		// }
 		sprintf(curr_write,"%s", find_value(x_help[i]));
 		check = write(fd_dc, find_value(x_help[i]), strlen(find_value(x_help[i])));
 		if(check<0){
 			perror("write");
 			return;
 		}
 		// check = write(fd_dc, ") = ", strlen(") = "));
 		// if(check<0){
 		// 	perror("write");
 		// 	return;
 		// }
 		check = write(fd_dc, "  ", strlen("  "));
 		if(check<0){
 			perror("write");
 			return;
 		}
 		sprintf(curr_write,"%.14e", gsl_vector_get(x,i));
 		check = write(fd_dc, curr_write, strlen(curr_write));
 		if(check<0){
 			perror("write");
 			return;
 		}
 		// check = write(fd_dc, " Volt\n", strlen(" Volt\n"));
 		// if(check<0){
 		// 	perror("write");
 		// 	return;
 		// }
 		check = write(fd_dc, "\n", strlen("\n"));
 		if(check<0){
 			perror("write");
 			return;
 		}
	}

	

	close(fd_dc);
}


	

int tran_init(struct element *head){
	int pre_counter = vol_counter;
	if(if_sparse){
		printf("JUST WAYYYY!!!!\n");
	}else{

		printf("Constructs the C matrix \n");
		constructor_tran_C(nodes(),m2_elem(), head );
		
		
		gsl_matrix_memcpy(mna_curr,mna);

		gsl_vector_memcpy(e_prev, b);
		gsl_vector_memcpy(b_curr, b);
		
		if(!if_BE) {
			compute_mna_transient(C, 2/time_step, nodes(), m2_elem());
		}
		else {
			compute_mna_transient(C, 1/time_step, nodes(), m2_elem() );
		}
	}
  	vol_counter = pre_counter;

  	return(-1);
}

int tran_free(){
	gsl_matrix_free(C);
	gsl_matrix_free(mna_curr);
	gsl_matrix_free(mna_tran);
	gsl_vector_free(e);
	gsl_vector_free(e_prev);
	gsl_vector_free(b_curr);

  	return(-1);
}

int tran_sol(struct element *head, unsigned long int *print_node, int index_print, FILE ** fd){
	int i, j,check;
	double value;
	double timestamp = time_step;
	int position = -1;
	if(if_sparse){
		printf("Constructs the MNA matrix and b vector for sparse matrix with transient analysis\n");
		
		//Construct C matrix
		constructor_C_sparse(nodes(), m2_elem(), head);
		mna_curr_sparse = memcopy_compressed_matrices(mna_curr_sparse, compressed_MNA, nodes(), m2_elem());
		memcpy(e_prev_sparse, b_sparse,(nodes() + m2_elem()-1)*sizeof(double));
		memcpy(b_curr_sparse, b_sparse,(nodes() + m2_elem()-1)*sizeof(double));

		//compute mna with trap or euler
		if(!if_BE) {
			compute_mna_transient_sparse(-1.0, nodes(), m2_elem());
		}
		else {
			compute_mna_transient_sparse(1.0, nodes(), m2_elem() );
		}

		while(timestamp <= fin_time) {
			compressed_MNA = memcopy_compressed_matrices(compressed_MNA, MNA_tran_sparse, nodes(), m2_elem());
			
			memcpy(b_sparse, b_curr_sparse,(nodes() + m2_elem() - 1)*sizeof(double));
			
			//upologismos mna gia kathe timestamp
			constructor_tran_sparse(nodes(),m2_elem(), head, timestamp); //calculates the e(tk) and not the b 
			
			memcpy(e_sparse, b_sparse,(nodes() + m2_elem()-1)*sizeof(double));  //the above save the e(tk) to the b, so we must copy it to the right variable
			b_tran_constructor_sparse(nodes(),m2_elem(), time_step);
			

			if(if_cholesky){
				printf("Computing x with Cholesky analysis \n");
				sparse_Cholesky_analysis(nodes(),m2_elem());
			}
			else if(if_CG){
				printf("Computing x with CG analysis \n");
				sparse_CG_analysis(nodes(),m2_elem());
			}
			else if(if_Bi_CG){
				printf("Computing x with Bi_CG analysis \n");
				sparse_Bi_CG_analysis(nodes(),m2_elem());
			}
			else{
				printf("Computing x with LU analysis \n");
			 	sparse_LU_analysis(nodes(),m2_elem());
			}

		
			
			for(j=0; j<index_print;j++){
				for(i=0;i<(nodes()-1); i++){
					if(x_help[i] == print_node[j]){
						position = i;
						break;
					}
				}
				fprintf(fd[j], "%.14lf ",timestamp);
				value = x_sparse[position];
				fprintf(fd[j], "%.14lf \n",value);
				
			}

			timestamp += time_step;
		}

    	memcpy(b_sparse, b_curr_sparse, (nodes() + m2_elem()-1)*sizeof(double));
		compressed_MNA = memcopy_compressed_matrices(compressed_MNA, mna_curr_sparse, nodes(), m2_elem());

		free(b_curr_sparse);
		free(e_prev_sparse);
	}
	else{
		gsl_vector_memcpy(e_prev, b);
		
	 	while(timestamp <= fin_time) {
			gsl_matrix_memcpy(mna, mna_tran);
			gsl_vector_memcpy(b, b_curr);

			constructor_tran(nodes(),m2_elem(), head, timestamp); //calculates the e(tk) and not the b 
			
			gsl_vector_memcpy(e,b);  //the above save the e(tk) to the b, so we must copy it to the right variable


			b_tran_constructor(nodes(),m2_elem(), time_step);

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
			 	check = LU_analysis(nodes(),m2_elem());
			 	if(check == -1){
					printf("Mulfunction with Bi_CG analysis \n");
					return -1;
				}
			}
			for(j=0; j<index_print;j++){
				for(i=0;i<(nodes()-1); i++){
					if(x_help[i] == print_node[j]){
						position = i;
						break;
					}
				}
				fprintf(fd[j], "%.14lf ",timestamp);
				value = gsl_vector_get(x,position);
				fprintf(fd[j], "%.14lf \n",value);
				
			}
			timestamp += time_step;
		}

	    gsl_vector_memcpy(b,b_curr);
	    gsl_matrix_memcpy(mna,mna_curr);
	}
	return 0;
}


