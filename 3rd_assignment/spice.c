#include "spice.h"

struct node * hash_table[HASH_TABLE_SIZE];

gsl_matrix *mna;
gsl_matrix *C;
gsl_vector *b;
gsl_vector *x;
unsigned long int *x_help;
cs_di *C_sparse;
cs_di *MNA_tran_sparse;
cs_di *mna_curr_sparse;



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

	if(if_tran) {
		tran_sol(head);
	}


	

	//free the structurs
	if(if_sparse) {
		free_mna_sparse();
		cs_di_spfree(compressed_MNA);
	}
	else {
		free_mna();
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

int tran_sol(struct element *head) {
	int i, check;
	double timestamp = time_step;

	if(if_sparse){
		printf("Constructs the MNA matrix and b vector for sparse matrix with transient analysis\n");
		
		//Construct C matrix
		constructor_C_sparse(nodes(), m2_elem(), head);
//		cs_di_spfree( mna_curr_sparse );

		// cs_di *temp;
		// temp = cs_di_spalloc( (nodes()+ m2_elem()-1), (nodes()+ m2_elem()-1), non_zeros, 1, 1);
		// mna_curr_sparse = cs_di_compress(temp);
	 //    cs_di_spfree(temp);
	 //    cs_di_dupl(mna_curr_sparse);

		// mna_curr_sparse = cs_di_add(mna_curr_sparse,compressed_MNA, 0.0, 1.0);
		mna_curr_sparse = memcopy_compressed_matrices(mna_curr_sparse, compressed_MNA, nodes(), m2_elem());

		// printf("----C----\n");
		// cs_di_print (mna_curr_sparse,0);
		// cs_di_print (compressed_MNA,0);
		
		// return 0;
		
    	//mna_curr_sparse = compressed_MNA;
    	

		// memcopy_compressed_matrices(mna_curr_sparse, compressed_MNA, nodes(), m2_elem());

		memcpy(e_prev_sparse, b_sparse,(nodes() + m2_elem()-1)*sizeof(double));

		memcpy(b_curr_sparse, b_sparse,(nodes() + m2_elem()-1)*sizeof(double));


		//compute mna with trap or euler
		if(!if_BE) {
			compute_mna_transient_sparse(-1.0, nodes(), m2_elem());
		}
		else {
			compute_mna_transient_sparse(1.0, nodes(), m2_elem() );
		}


		// printf("----C----\n");
		// cs_di_print (compressed_MNA,0);
		while(timestamp <= fin_time) {
			compressed_MNA = memcopy_compressed_matrices(compressed_MNA, MNA_tran_sparse, nodes(), m2_elem());
			
			memcpy(e_prev_sparse, b_sparse,(nodes() + m2_elem() - 1)*sizeof(double));
			
			//upologismos mna gia kathe timestamp
			constructor_tran_sparse(nodes(),m2_elem(), head, timestamp); //calculates the e(tk) and not the b 
			
			memcpy(e_sparse, b_sparse,(nodes() + m2_elem()-1)*sizeof(double));  //the above save the e(tk) to the b, so we must copy it to the right variable
			b_tran_constructor_sparse(nodes(),m2_elem(), time_step);
			
			printf("Computing x with LU analysis \n");
     		sparse_LU_analysis(nodes(), m2_elem());
			for(i = 0; i<(nodes() + m2_elem()-1); i++) {
				printf("%d: %lf\n", i, x_sparse[i] );
			}

			timestamp += time_step;
		}
		// print_x();	
		// plot(head);
    	memcpy(b_sparse, b_curr_sparse, (nodes() + m2_elem()-1)*sizeof(double));
		free(b_curr_sparse);
		free(e_prev_sparse);
	}
	else{

		// make mna
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

		while(timestamp <= fin_time) {
			printf("time_step : %lf, fin_time %lf\n",time_step, fin_time );
			gsl_matrix_memcpy(mna, mna_tran);
			gsl_vector_memcpy(b, e_prev);

			constructor_tran(nodes(),m2_elem(), head, timestamp); //calculates the e(tk) and not the b 
			
			gsl_vector_memcpy(e,b);  //the above save the e(tk) to the b, so we must copy it to the right variable


			b_tran_constructor(nodes(),m2_elem(), time_step);

			print_MNA(nodes(),m2_elem());
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
				// printf("-----------------------vector_b before------------------\n");
	   //          for(i=0; i<(nodes()+m2_elem()-1); i++){
	   //              printf("%d: %lf\n",i, gsl_vector_get(x,i));
	   //          }
				// for(i=0; i<(nodes()+m2_elem()-1); i++){
	   //              gsl_vector_set(x,i,0.0);
	   //          }
				printf("Computing x with LU analysis \n");
			 	LU_analysis(nodes(),m2_elem());

				printf("\nVector x:%lf\n\n",timestamp);
            	for(i=0; i<(nodes()+m2_elem()-1); i++){
                	printf("%d: %lf\n",i, gsl_vector_get(x,i));
            	}
			}
			// print_x();
			// //handles PRINT|PLOT if exist in netlist
			// plot(head);	

			timestamp += time_step;
			// gsl_matrix_memcpy(mna,mna_curr);
		}
	    gsl_vector_memcpy(b,b_curr);
        gsl_vector_free(b_curr);
        gsl_vector_free(e_prev);
	    
		
	}
	return 0;
}