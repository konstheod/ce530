#include "spice.h"


struct node * hash_table[HASH_TABLE_SIZE];

int counter_m2 = 0; //counter for elements in team2
int index_print = 0; //counts element for plot
int non_zeros = 0;
gsl_vector *x;
unsigned long int *x_help;

int parser(struct element **element_head, int fd){
	int k, i, j=1, check;
	char el_type, name[SIZE_NAME];
	char curr_value[SIZE_VALUE], input[1000], curr_area[SIZE_VALUE];
	char node_name[NODE_SIZE];	
	struct element *head, *curr, *tail, *dc_curr;
	head = *element_head;
	tail = NULL;
	curr = NULL;

	if_cholesky = 0;
	if_CG = 0;
	if_Bi_CG = 0;
	itol = 1e-3;
	if_sparse = 0;
	if_BE = 0;
	if_tran = 0;


	while(1){
		check = read(fd, input, 1000);
		
		if(check<0){
			printf("Problem with read\n");
			close(fd);
			return(-1);
		}
		i = 0;

		while(i<check){

			if(input[i] == '\n'){
				j++;
				i++;
				continue;
			}
			else if(input[i] == '*'){
				while(i<check && input[i]!='\n'){
					i++;
					if(check==i){
						check = read(fd, input, 1000);
		
						if(check<0){
							printf("Problem with read\n");
							close(fd);
							return(-1);
						}
						i = 0;
					}
				}
				continue;
			}
			else if(input[i] == ' ' || input[i] == '\t'){
				i++;
				continue;
			}
			else if(input[i] == '.'){
				i++;

				if(check==i){
					check = read(fd, input, 1000);
	
					if(check<0){
						printf("Problem with read\n");
						close(fd);
						return(-1);
					}
					i = 0;
				}
				if(input[i] == 'D') {
					//einai DC
					for(k = 0; k<2; k++){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
			
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}
					while(input[i] == '\t' || input[i] == ' '){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}
					el_type = input[i];
					el_type = toupper(el_type);
					i++;
					if(check==i){
						check = read(fd, input, 1000);
						if(check<0){
							printf("Problem with read\n");
							close(fd);
							return(-1);
						}
						i = 0;
					}
					for(k=0; k<SIZE_NAME; k++){
						name[k] = '\0';
					}
					k = 0;
					while(input[i] != '\t' && input[i] != ' ') {
						name[k] = input[i];
						k++;
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}
					for(dc_curr = head; dc_curr!=NULL; dc_curr = dc_curr->next){
						if(el_type == dc_curr->type && strcmp(dc_curr->name,name) == 0){
							while(input[i] == '\t' || input[i] == ' '){
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}
							
							for(k=0; k<SIZE_VALUE; k++){
								curr_value[k] = '\0';
							}
							k = 0;
							while(input[i] != '\t' && input[i] != ' ' && input[i]!='\n') {

								curr_value[k] = input[i];
								k++;
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}

							dc_curr->start_value = atof(curr_value);

							while(input[i] == '\t' || input[i] == ' '){
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}
							for(k=0; k<SIZE_VALUE; k++){
								curr_value[k] = '\0';
							}
							k = 0;
							while(input[i] != '\t' && input[i] != ' ' && input[i]!='\n') {

								curr_value[k] = input[i];
								k++;
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}

							dc_curr->end_value = atof(curr_value);

							while(input[i] == '\t' || input[i] == ' '){
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}
							for(k=0; k<SIZE_VALUE; k++){
								curr_value[k] = '\0';
							}
							k = 0;
							while(input[i] != '\t' && input[i] != ' ' && input[i]!='\n') {

								curr_value[k] = input[i];
								k++;
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}

							dc_curr->increment = atof(curr_value);
							dc_curr->dc = 1;
							continue;
						}
					}
				}
				else if(input[i] == 'P') {
					//plot 'h print

					i++;
					if(check==i){
						check = read(fd, input, 1000);
		
						if(check<0){
							printf("Problem with read\n");
							close(fd);
							return(-1);
						}
						i = 0;
					}
					int str_size = 4;
					if(input[i] == 'L'){
						str_size = 3;
					}
					for(k = 0; k<str_size; k++){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
			
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}
					while(input[i]!='\n' && input[i]!= EOF){
						while(input[i] == '\t' || input[i] == ' '){
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
						}
						if(input[i] == 'V'){
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
							if(input[i] == '('){
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
								for(k=0;k<NODE_SIZE;k++){
									node_name[k]='\0';
								}
								k = 0;
								while(input[i] != '\t' && input[i] != ' ' && input[i]!=')') {
									node_name[k] = input[i];
									k++;
									i++;
									
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
								}
								//malloc kai tha vazoume ston if_print to hash	
								if(index_print == 0){
									if_print = (unsigned long int *) malloc(sizeof(unsigned long int));
								} 
								else {
									if_print = (unsigned long int *) realloc(if_print,sizeof(unsigned long int) * (index_print + 1));
								}
								if_print[index_print] = find_node(node_name);
								index_print++;
								
								while(input[i] == '\t' || input[i] == ' ' || input[i]==')'){
									i++;
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										else if(check == 0){
											goto exit;
										}
										i = 0;
									}
								}
							}
						}
					}
				}
				else if(input[i] == 'O') {
					for(k = 0; k<7; k++){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
			
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					while(input[i]!='\n') {
						while(input[i] == '\t' || input[i] == ' '){
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
						}
						for(k=0;k<NODE_SIZE;k++){
							node_name[k]='\0';
						}
						k = 0;
						while(input[i] != '\t' && input[i] != ' ' && input[i] != '\n') {
							node_name[k] = input[i];
							k++;
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
						}
						if(!strcmp(node_name,"SPARSE")) {
							if_sparse = 1;
							printf("SPARSE\n");
						}
						else if(!strcmp(node_name,"SPD")) {
							if(if_Bi_CG){
								if_CG = 1;
								if_Bi_CG = 0;
								printf("ITER SPD\n");
							}
							else {
								if_cholesky = 1;
								printf("SPD\n");
							}
						}
						else if(!strcmp(node_name,"ITER")) {
							if(if_cholesky) {
								if_cholesky = 0;
								if_CG = 1;
								printf("ITER SPD\n");
							}
							else {
								if_Bi_CG = 1;
								printf("ITER\n");
							}

						}
						else if(!strcmp(node_name,"ITOL")) {
							while(input[i] == '\t' || input[i] == ' ' || input[i] == '='){
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}

							for(k=0;k<NODE_SIZE;k++){
								node_name[k]='\0';
							}
							k = 0;
							while(input[i] != '\t' && input[i] != ' ' && input[i] != '=' && input[i] != '\n') {
								node_name[k] = input[i];
								k++;
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}
							itol  = atof(node_name);
							printf("ITOL = %lf\n", itol);
						}
						else if(!strcmp(node_name,"METHOD")) {
							while(input[i] == '\t' || input[i] == ' ' || input[i] == '='){
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}

							for(k=0;k<NODE_SIZE;k++){
								node_name[k]='\0';
							}
							k = 0;
							while(input[i] != '\t' && input[i] != ' ' && input[i] != '=' && input[i] != '\n') {
								node_name[k] = input[i];
								k++;
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}
							
							if(!strcmp(node_name,"BE")) {
								if_BE = 1;
								printf("BE\n");
							}
							else{
								printf("TR\n");
							}

						}

					}		
				}
				else if(input[i] == 'T') {
					if_tran = 1;
					for(k = 0; k<4; k++){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
			
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}
					while(input[i] == '\t' || input[i] == ' '){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}
					for(k=0;k<NODE_SIZE;k++){
						node_name[k]='\0';
					}
					k = 0;
					while(input[i] != '\t' && input[i] != ' ' && input[i] != '\n') {
						node_name[k] = input[i];
						k++;
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					time_step = atof(node_name);

					while(input[i] == '\t' || input[i] == ' '){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}
					for(k=0;k<NODE_SIZE;k++){
						node_name[k]='\0';
					}
					k = 0;
					while(input[i] != '\t' && input[i] != ' ' && input[i] != '\n') {
						node_name[k] = input[i];
						k++;
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					fin_time = atof(node_name);
					printf("time_step = %lf fin_time = %lf \n",time_step, fin_time );
				}
			}
			else{
				input[i] = toupper(input[i]); // ta kanei ola kefalaia

				curr = (struct element *) malloc (sizeof(struct element));
				if(curr == NULL){
					printf("Error to malloc!\n");
					return(-1);
				}
				curr->transient_spec = -1;
				
				if(head != NULL){
					if(tail!=NULL){
						tail->next = curr;
						curr->next = NULL;
						tail = curr;
					}
					else{
						tail = curr;
						head->next = curr;
						tail->next = NULL;
					}
				} 
				else {
					head = curr;
					curr->next = NULL;
				}

				if(input[i] == 'R' || input[i] == 'C' || input[i] == 'L' || input[i] == 'V' || input[i] == 'I'){
					curr->type = input[i];

					//find max_non_zeroes elements for sparse
					if(input[i] == 'R'){
						non_zeros += 4;
					}

					if(input[i] == 'V' || input[i] == 'L'){
						counter_m2++;
						non_zeros += 2;
					}

					i++;
					if(check==i){
						check = read(fd, input, 1000);
		
						if(check<0){
							printf("Problem with read\n");
							close(fd);
							return(-1);
						}
						i = 0;
					}
					//init curr_value
					for(k=0; k<SIZE_VALUE; k++){
						curr_value[k] = '\0';
					}
					//find element name
					k = 0;
					while(input[i] != '\t' && input[i] != ' ') {
						curr->name[k] = input[i];
						k++;
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					while(input[i] == '\t' || input[i] == ' '){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					//find positive probe
					for(k=0;k<NODE_SIZE;k++){
						node_name[k]='\0';
					}
					k = 0;
					while(input[i] != '\t' && input[i] != ' ') {
						node_name[k] = input[i];
						k++;
						i++;
						
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}
					curr->pos = add_node(node_name);

					while(input[i] == '\t' || input[i] == ' '){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					//find negative probe
					for(k=0;k<NODE_SIZE;k++){
						node_name[k]='\0';
					}
					k = 0;
					while(input[i] != '\t' && input[i] != ' ') {
						node_name[k] = input[i];
						k++;
						i++;
						
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}
					curr->neg = add_node(node_name);

					while(input[i] == '\t' || input[i] == ' '){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					k = 0;
					while(input[i] != '\t' && input[i] != ' ' && input[i]!='\n') {

						curr_value[k] = input[i];
						k++;
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					curr->value = atof(curr_value);

					//looking for transient_spec
					if((curr->type == 'I' || curr->type== 'V') && input[i]!='\n'){

						while(input[i] == '\t' || input[i] == ' '){
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
							
						}

						for(k=0;k<NODE_SIZE;k++) {
							node_name[k]='\0';
						}
						k = 0;
						while(input[i] != '\t' && input[i] != ' ' && input[i] != '(') {
							node_name[k] = input[i];
							k++;
							i++;
							
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
						}

						if(!strcmp(node_name,"EXP")) {

							curr->exp_spec = (struct spec_exp *) malloc (sizeof(struct spec_exp));
							if(curr->exp_spec == NULL){
								printf("Error to malloc!\n");
								return(-1);
							}

							curr->transient_spec = EXP;

							for(j = 0; j < 6; j++){

								while(input[i] == '\t' || input[i] == ' ' || input[i] == '('){
									i++;
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
								}

								for(k=0;k<SIZE_VALUE;k++) {
									curr_value[k]='\0';
								}
								k = 0;
								while(input[i] != '\t' && input[i] != ' ' && input[i] != '(' && input[i] != ')') {
									curr_value[k] = input[i];
									k++;
									i++;
									
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
								}

								if(j == 0){
									curr->exp_spec->value1 = atof(curr_value);
									// printf("value1 = %lf\n", curr->exp_spec->value1);
								}
								else if(j == 1){
									curr->exp_spec->value2 = atof(curr_value);
									// printf("value2 = %lf\n", curr->exp_spec->value2);
								}
								else if(j == 2){
									curr->exp_spec->td1 = atof(curr_value);
									// printf("td1 = %lf\n", curr->exp_spec->td1);
								}
								else if(j == 3){
									curr->exp_spec->tc1 = atof(curr_value);
									// printf("tc1 = %lf\n", curr->exp_spec->tc1);
								}
								else if(j == 4){
									curr->exp_spec->td2 = atof(curr_value);
									// printf("td2 = %lf\n", curr->exp_spec->td2);
								}
								if(j == 5){
									curr->exp_spec->tc2 = atof(curr_value);
									// printf("tc2 = %lf\n", curr->exp_spec->tc2);
								}
							}
						} 
						else if(!strcmp(node_name,"SIN")) {

							curr->sin_spec = (struct spec_sin *) malloc (sizeof(struct spec_sin));
							if(curr->sin_spec == NULL){
								printf("Error to malloc!\n");
								return(-1);
							}

							curr->transient_spec = SIN;

							for(j = 0; j < 6; j++){

								while(input[i] == '\t' || input[i] == ' ' || input[i] == '('){
									i++;
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
								}

								for(k=0;k<SIZE_VALUE;k++) {
									curr_value[k]='\0';
								}
								k = 0;
								while(input[i] != '\t' && input[i] != ' ' && input[i] != '(' && input[i] != ')') {
									curr_value[k] = input[i];
									k++;
									i++;
									
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
								}

								if(j == 0){
									curr->sin_spec->value1 = atof(curr_value);
									// printf("value1 = %lf\n", curr->sin_spec->value1);
								}
								else if(j == 1){
									curr->sin_spec->valuea = atof(curr_value);
									// printf("valuea = %lf\n", curr->sin_spec->valuea);
								}
								else if(j == 2){
									curr->sin_spec->fr = atof(curr_value);
									// printf("fr = %lf\n", curr->sin_spec->fr);
								}
								else if(j == 3){
									curr->sin_spec->td = atof(curr_value);
									// printf("td = %lf\n", curr->sin_spec->td);
								}
								else if(j == 4){
									curr->sin_spec->df = atof(curr_value);
									// printf("df = %lf\n", curr->sin_spec->df);
								}
								if(j == 5){
									curr->sin_spec->ph = atof(curr_value);
									// printf("ph = %lf\n", curr->sin_spec->ph);
								}
							}
						}
						else if(!strcmp(node_name,"PULSE")) {

							curr->pulse_spec = (struct spec_pulse *) malloc (sizeof(struct spec_pulse));
							if(curr->pulse_spec == NULL){
								printf("Error to malloc!\n");
								return(-1);
							}

							curr->transient_spec = PULSE;

							for(j = 0; j < 7; j++){

								while(input[i] == '\t' || input[i] == ' ' || input[i] == '('){
									i++;
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
								}

								for(k=0;k<SIZE_VALUE;k++) {
									curr_value[k]='\0';
								}
								k = 0;
								while(input[i] != '\t' && input[i] != ' ' && input[i] != '(' && input[i] != ')') {
									curr_value[k] = input[i];
									k++;
									i++;
									
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
								}

								if(j == 0){
									curr->pulse_spec->value1 = atof(curr_value);
									// printf("value1 = %lf\n", curr->pulse_spec->value1);
								}
								else if(j == 1){
									curr->pulse_spec->value2 = atof(curr_value);
									// printf("value2 = %lf\n", curr->pulse_spec->value2);
								}
								else if(j == 2){
									curr->pulse_spec->td = atof(curr_value);
									// printf("td = %lf\n", curr->pulse_spec->td);
								}
								else if(j == 3){
									curr->pulse_spec->tr = atof(curr_value);
									// printf("tr = %lf\n", curr->pulse_spec->tr);
								}
								else if(j == 4){
									curr->pulse_spec->tf = atof(curr_value);
									// printf("tf = %lf\n", curr->pulse_spec->tf);
								}
								else if(j == 5){
									curr->pulse_spec->pw = atof(curr_value);
									// printf("pw = %lf\n", curr->pulse_spec->pw);
								}
								if(j == 6){
									curr->pulse_spec->per = atof(curr_value);
									// printf("per = %ld\n", curr->pulse_spec->per);
								}
							}
							curr->pulse_spec->k = 0;
							curr->pulse_spec->old_value = curr->value;
						} 
						else if(!strcmp(node_name,"PWL")) {
							int new_line = 0;

							curr->pwl_spec = (struct spec_pwl *) malloc (sizeof(struct spec_pwl));
							if(curr->pwl_spec == NULL){
								printf("Error to malloc!\n");
								return(-1);
							}

							curr->transient_spec = PWL;
							j = 0;
							while(1) {

								while(input[i] == '\t' || input[i] == ' ' || input[i] == '(' || input[i] == '\n' || input[i] == ')'){
									i++;
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
									if(input[i] == '\n')
									{
										new_line = 1;
										curr->pwl_spec_len = j-1;
										break;
									}
								}

								if(new_line) {
									break;
								}

								if(j != 0){
									curr->pwl_spec = (struct spec_pwl *) realloc(curr->pwl_spec,sizeof(struct spec_pwl) * (j + 1));
								} 

								for(k=0;k<SIZE_VALUE;k++) {
									curr_value[k]='\0';
								}
								k = 0;
								while(input[i] != '\t' && input[i] != ' ') {
									curr_value[k] = input[i];
									k++;
									i++;
									
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
								}

								curr->pwl_spec[j].t = atof(curr_value);
								// printf("%d: pwl->time  = %lf \n", j, curr->pwl_spec[j].t);

								while(input[i] == '\t' || input[i] == ' '){
									i++;
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
								}

								for(k=0;k<SIZE_VALUE;k++) {
									curr_value[k]='\0';
								}
								k = 0;
								while(input[i] != '\t' && input[i] != ' ' && input[i] != ')') {
									curr_value[k] = input[i];
									k++;
									i++;
									
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
								}

								curr->pwl_spec[j].value = atof(curr_value);
								// printf("%d: pwl->value  = %lf \n", j, curr->pwl_spec[j].value);

								while(input[i] == '\t' || input[i] == ' ' || input[i] == ')' || input[i] == '\n'){
									i++;
									if(check==i){
										check = read(fd, input, 1000);
										if(check<0){
											printf("Problem with read\n");
											close(fd);
											return(-1);
										}
										i = 0;
									}
									if(input[i] == '\n')
									{
										new_line = 1;
										curr->pwl_spec_len = j;
										break;
									}
								}

								j++;
							}
						}


						while(input[i] == '\t' || input[i] == ' ' || input[i]==')'){
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
						}

					}
					curr->dc = 0;
					continue;


				}
				else if(input[i] == 'D' || input[i] == 'M' || input[i] == 'Q') {
					curr->type = input[i];
					i++;
					if(check==i){
						check = read(fd, input, 1000);
		
						if(check<0){
							printf("Problem with read\n");
							close(fd);
							return(-1);
						}
						i = 0;
					}

					//init curr_value
					for(k=0; k<SIZE_VALUE; k++){
						curr_value[k] = '\0';
					}
					//find element name
					k = 0;
					while(input[i] != '\t' && input[i] != ' ') {
						curr->name[k] = input[i];
						k++;
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}
					while(input[i] == '\t' || input[i] == ' '){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					//find first probe
					for(k=0;k<NODE_SIZE;k++){
						node_name[k]='\0';
					}
					k = 0;
					while(input[i] != '\t' && input[i] != ' ') {
						node_name[k] = input[i];
						k++;
						i++;
						
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					if(curr->type == 'D'){
						curr->pos = add_node(node_name);
					}
					else if(curr->type == 'Q'){
						curr->C = add_node(node_name);	
					}
					else{
						curr->D = add_node(node_name);	
					}


					while(input[i] == '\t' || input[i] == ' '){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					//find second probe
					for(k=0;k<NODE_SIZE;k++){
						node_name[k]='\0';
					}
					k = 0;
					while(input[i] != '\t' && input[i] != ' ') {
						node_name[k] = input[i];
						k++;
						i++;
						
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					if(curr->type == 'D'){
						curr->neg = add_node(node_name);
					}
					else if(curr->type == 'Q'){
						curr->B = add_node(node_name);	
					}
					else{
						curr->G = add_node(node_name);	
					}


					//if not diode it has at least one more probe
					if(curr->type != 'D'){
						while(input[i] == '\t' || input[i] == ' '){
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
						}

						//find third probe
						for(k=0;k<NODE_SIZE;k++){
							node_name[k]='\0';
						}
						k = 0;
						while(input[i] != '\t' && input[i] != ' ') {
							node_name[k] = input[i];
							k++;
							i++;
							
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
						}

						if(curr->type == 'Q'){
							curr->E = add_node(node_name);	
						}
						else{
							curr->S = add_node(node_name);
							while(input[i] == '\t' || input[i] == ' '){
								i++;
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}

							//find forth probe
							for(k=0;k<NODE_SIZE;k++){
								node_name[k]='\0';
							}
							k = 0;
							while(input[i] != '\t' && input[i] != ' ') {
								node_name[k] = input[i];
								k++;
								i++;
								
								if(check==i){
									check = read(fd, input, 1000);
									if(check<0){
										printf("Problem with read\n");
										close(fd);
										return(-1);
									}
									i = 0;
								}
							}

							curr->B = add_node(node_name);
						}

					}

					//find whitespaces before model name
					while(input[i] == '\t' || input[i] == ' '){
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					//finds model name
					k = 0;
					while(input[i] != '\t' && input[i] != ' ') {
						curr->model_name[k] = input[i];
						k++;
						i++;
						if(check==i){
							check = read(fd, input, 1000);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					if(curr->type != 'M'){
						//find whitespaces before area if exists
						while(input[i] == '\t' || input[i] == ' '){
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
						}
						k = 0;
						//init curr_area
						for(k=0; k<SIZE_VALUE; k++){
							curr_area[k] = '\0';
						}
						k = 0;
						while(input[i] != '\n' && input[i] != '\t' && input[i] != ' ' ){
							curr_area[k] = input[i];
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
							k++;
						}
						if(strlen(curr_area)>0){
							curr->area = atof(curr_area);
						}
						else{
							curr->area = 1.0;	
						}
						curr->dc = 0;
					}
					else{ // it is mos transistor
						//find whitespaces before L
						while(!isdigit(input[i])){
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
						}
						
							//init curr_value
						for(k=0; k<SIZE_VALUE; k++){
							curr_value[k] = '\0';
						}

						k = 0;
						while(input[i] != '\t' && input[i] != ' '){
							curr_value[k] = input[i];
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
							k++;
						}
						curr->L = atof(curr_value);

						//find whitespaces before W
						while(!isdigit(input[i])){
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
						}

							//init curr_value
						for(k=0; k<SIZE_VALUE; k++){
							curr_value[k] = '\0';
						}

						k = 0;
						while(input[i] != '\t' && input[i] != ' ' && input[i] != '\n'){
							curr_value[k] = input[i];
							i++;
							if(check==i){
								check = read(fd, input, 1000);
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
							k++;
						}
						curr->W = atof(curr_value);
						curr->dc = 0;
					}
				}
				
			}


			i++;
		}
		//End of file reached
		if(check < 100 ){
			break;
		}
	}
exit:	
	*element_head = head;
	return(1);
}

int free_elements(struct element **element_head){
	struct element *head, *next;
	head = *element_head;
	
	while(head!=NULL){
		next = head->next;
		free(head);
		head = next;
	}
	
	*element_head = head;
	free(if_print);
	return(1);
}

void printList(struct element *head){
	struct element *curr;
	int i = 1;
	
	for(curr = head; curr != NULL; curr = curr->next){
		if(curr->type == 'R' || curr->type == 'C' || curr->type == 'L' || curr->type == 'V' || curr->type == 'I'){

			printf("%d: %c%s %s %s %.*lf \n",i, curr->type, curr->name, find_value(curr->pos), find_value(curr->neg), PREC,curr->value);
			
		}
		else if(curr->type == 'D'){
			printf("%d: %c%s %s %s %s %.*lf \n", i, curr->type, curr->name, find_value(curr->pos), find_value(curr->neg), curr->model_name, PREC, curr->area);
		}
		else if(curr->type == 'M'){
			printf("%d: %c%s %s %s %s %s %s L = %.*lf W = %.*lf \n", i, curr->type, curr->name, find_value(curr->D), find_value(curr->G), find_value(curr->S), find_value(curr->B), curr->model_name, PREC, curr->L, PREC, curr->W);
		}
		else {
			printf("%d: %c%s %s %s %s %s %.*lf \n", i, curr->type, curr->name, find_value(curr->C), find_value(curr->B), find_value(curr->E), curr->model_name, PREC, curr->area);
		}
		i++;
	}
	for(curr = head; curr != NULL; curr = curr->next){
		if(curr->type == 'V' || curr->type == 'I'){
			if(curr->dc){
				printf("%d: .DC %c%s %lf %lf %.*lf \n",i, curr->type, curr->name, curr->start_value,  curr->end_value, PREC, curr->increment);
			}
		}
	}
				
	
}

int m2_elem(){
	return counter_m2;
}

int plot(struct element *head){
	int i, k;
	double j, x_get;
	struct element *curr;
	int check;
	char file_name[33];
	char def_file_name[9];
	int fd;
	char curr_write[20];
	int new_file;
	int lim;
	int t;
	FILE * file_fd;
	FILE **plot_files;
	char **plot_names;

	strcpy(def_file_name,"node_name");
	strcpy(file_name,def_file_name);


	//loop for all the print elements
	for(i = 0; i < index_print; i++){
		new_file = 0;
		//loop for all elements of the list
		for(curr = head; curr!=NULL; curr = curr->next){
			//check if an element has DC
			if(curr->dc == 0){
				continue;
			}
			//initialization of file_name
			for(k = 9; k < 33; k++){
				file_name[k] = '\0';
			}

			//construct the correct file name
			strcat(file_name,find_value(if_print[i]));
			strcat(file_name,"_");
			sprintf(curr_write,"%c", curr->type);
			strcat(file_name,curr_write);
			strcat(file_name,curr->name);
			strcat(file_name,".txt");
			//if the file exists change the name
			while(1){
				fd = open(file_name, O_RDONLY, 0777);
				if(fd>0){
					new_file++;
					for(k = strlen(find_value(if_print[i])) + 9; k < strlen(file_name); k++){
						file_name[k] = '\0';
					}
					strcat(file_name,"_");
					sprintf(curr_write,"%c", curr->type);
					strcat(file_name,curr_write);
					strcat(file_name,curr->name);
					strcat(file_name,"(");
					sprintf(curr_write,"%d", new_file);
					strcat(file_name,curr_write);
					strcat(file_name,").txt");
					close(fd);
				}
				else if(fd<0){
					fd = open(file_name, O_CREAT | O_WRONLY, 0777);
					if(fd < 0){
						perror("open");
						return -1;
					}
					break;
				}
			}
			lim = (curr->end_value-curr->start_value) / curr->increment;
			t = 0;
			j = curr->start_value;
			//make new b for each value of dc voltage source and print the result to file
			while(t < lim + 1){
				//construct b

				//solve the system Ax = b

				if(if_sparse) {
					if(curr->type == 'V'){
						MNA_voltage_dc_sparse(curr, j, nodes());
					}
					else if(curr->type == 'I'){
						if(j>curr->start_value){
							MNA_power_dc_sparse(curr, j, j-curr->increment);
						}
						else{
							MNA_power_dc_sparse(curr, j, curr->value);
						}
					}
					sparse_matrix(nodes(), m2_elem());
				}
				else {
					if(curr->type == 'V'){
						MNA_voltage_dc(curr, j, nodes());
					}
					else if(curr->type == 'I'){
						if(j>curr->start_value){
							MNA_power_dc(curr, j, j-curr->increment);
						}
						else{
							MNA_power_dc(curr, j, curr->value);
						}
					}
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
							return -1;
						}
					}
					else{
						printf("Computing x with LU analysis \n");
					 	LU_analysis(nodes(),m2_elem());
					}
				}

				//find the node you want to print
				for(k=0; k<(nodes()-1); k++){
					if(x_help[k] == if_print[i]){

						//write the results to file
						sprintf(curr_write,"%c", curr->type);
						check = write(fd, curr_write, strlen(curr_write));
						if(check<0){
							perror("write");
							return -1;
						}
						check = write(fd, curr->name, strlen(curr->name));
						if(check<0){
							perror("write");
							return -1;
						}
						check = write(fd, " = " , strlen(" = "));
						if(check<0){
							perror("write");
							return -1;
						}
						sprintf(curr_write,"%lf", j);
						check = write(fd, curr_write, strlen(curr_write));
						if(check<0){
							perror("write");
							return -1;
						}
						if(strlen(curr_write) < 9){
							check = write(fd, " " , strlen(" "));
							if(check<0){
								perror("write");
								return -1;
							}
						}
						for(check = 0; check < 20; check++){
							curr_write[check] = '\0';
						}
						check = write(fd, "\t" , strlen("\t"));
						if(check<0){
							perror("write");
							return -1;
						}
						check = write(fd, "V(" , strlen("V("));
						if(check<0){
							perror("write");
							return -1;
						}
						check = write(fd, find_value(if_print[i]) , strlen(find_value(if_print[i])));
						if(check<0){
							perror("write");
							return -1;
						}
						check = write(fd, ") = " , strlen(") = "));
						if(check<0){
							perror("write");
							return -1;
						}
						x_get = gsl_vector_get(x,k);
						sprintf(curr_write,"%lf", x_get);
						check = write(fd, curr_write , strlen(curr_write));
						if(check<0){
							perror("write");
							return -1;
						}
						for(check = 0; check < 20; check++){
							curr_write[check] = '\0';
						}
						check = write(fd, "\n", strlen("\n"));
						if(check<0){
							perror("write");
							return -1;
						}
					}
				}
				t++;
				j += curr->increment;
			}
			if(if_sparse){
				if(curr->type == 'V'){
					MNA_voltage_dc_sparse(curr, curr->value, nodes());
				}
				else if(curr->type == 'I'){
					MNA_power_dc_sparse(curr, curr->value, curr->end_value);
				}
			}
			else
			{
				if(curr->type == 'V'){
					MNA_voltage_dc(curr, curr->value, nodes());
				}
				else if(curr->type == 'I'){
					MNA_power_dc(curr, curr->value, curr->end_value);
				}
			}
			close(fd);
		}
	}

	if(!if_tran){
		return 0;
	}
	tran_init(head);

	strcpy(def_file_name,"node_");
	strcpy(file_name,def_file_name);

	//loop for all the print elements
	new_file = 0;

    plot_files = (FILE **)malloc(sizeof(FILE *) * index_print);
    plot_names = (char **)malloc(sizeof(char *) * index_print);

	for(i = 0; i < index_print; i++){
		new_file = 0;
		for(k = 5; k < 33; k++){
			file_name[k] = '\0';
		}
		//construct the correct file name
		strcat(file_name,find_value(if_print[i]));
		strcat(file_name,"_tran");
		strcpy(def_file_name, file_name);

		while(1){
			strcat(file_name,".txt");
			file_fd = fopen(file_name, "r");

			//fd = open(file_name, O_RDONLY, 0777);
			if(file_fd!=NULL){
				for(k = 0; k < 33; k++){
					file_name[k] = '\0';
				}
				new_file++;
				for(k = strlen(find_value(if_print[i])) + 9; k < strlen(file_name); k++){
					file_name[k] = '\0';
				}
				strcpy(file_name,def_file_name);

				strcat(file_name,"_");
				strcat(file_name,"(");
				sprintf(curr_write,"%d", new_file);
				strcat(file_name,curr_write);
				strcat(file_name,")");
				close(fd);
			}
			else if(file_fd == NULL){
				//printf("%s\n",file_name );
				file_fd = fopen(file_name, "w");
//				fd = open(file_name, O_CREAT | O_WRONLY, 0777);
				if(file_fd == NULL){
					perror("open");
					return -1;
				}
				plot_names[i] = (char *)malloc(sizeof(char ) * strlen(file_name));
				plot_files[i] = file_fd;
				strcpy(plot_names[i], file_name);
				// printf("%s\n",plot_names[i]);
				// printf("%p\n", file_fd);
				// printf("%p\n", plot_files[i]);
				break;
			}
		}	
	}
	
	tran_sol(head, if_print, index_print, plot_files);
	
	if(if_tran){
		FILE *pipe = NULL;
		char line[3000];
		pipe = popen( "gnuplot -persist", "w" );
		if(pipe == NULL){
			printf("Failed to open the Pipe\n");
			goto exit_parser;
		}
		//fprintf(temp, "\n");
		fprintf(pipe, "%s \n", "set title 'Transient Analysis'");
		fprintf(pipe, "%s \n", "set xlabel 'Steps'");
		fprintf(pipe, "%s \n", "set ylabel 'Voltage'");
		fprintf(pipe, "%s \n", "set grid");
		fprintf(pipe, "%s \n", "set zeroaxis");
		
		fprintf(pipe, "%s \n", "set key top right");
		fprintf(pipe, "%s \n", "set terminal wxt size 800,600");
		fprintf(pipe, "%s \n", "set autoscale");

		for(i = 0; i < index_print; i++){
			fflush(plot_files[i]);
			
			if(i!=0){

				line[0] = '\0';
				sprintf(line, "replot \'%s\' using 1:2  title \'%s\' with line", plot_names[i], find_value(if_print[i]));
				fprintf(pipe, "%s \n",line);

			}
			else{
				line[0] = '\0';
				sprintf(line, "plot \'%s\' using 1:2  title \'%s\' with line", plot_names[i], find_value(if_print[i]));
				fprintf(pipe, "%s \n",line);

			}
		}
exit_parser:

		for(i = 0; i < index_print; i++){
			fclose(plot_files[i]);
		}
		fclose(pipe);

	}
	close(fd);
	return 0;
}