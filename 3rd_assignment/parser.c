#include "spice.h"


struct node * hash_table[HASH_TABLE_SIZE];

int counter_m2 = 0; //counter for elements in team2
int index_print = 0; //counts element for plot

gsl_vector *x;
gsl_vector *x_help;

int parser(struct element **element_head, int fd){
	int k, i, j=1, check;
	char el_type, name[SIZE_NAME];
	char curr_value[SIZE_VALUE], input[100], curr_area[SIZE_VALUE];
	char node_name[NODE_SIZE];	
	struct element *head, *curr, *tail, *dc_curr;
	head = *element_head;
	tail = NULL;
	curr = NULL;

	if_cholesky = 0;

	while(1){
		check = read(fd, input, 100);
		
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
						check = read(fd, input, 100);
		
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
					check = read(fd, input, 100);
	
					if(check<0){
						printf("Problem with read\n");
						close(fd);
						return(-1);
					}
					i = 0;
				}
				if(input[i] == 'D'){
					//einai DC
					for(k = 0; k<2; k++){
						i++;
						if(check==i){
							check = read(fd, input, 100);
			
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
							check = read(fd, input, 100);
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
						check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
									check = read(fd, input, 100);
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
									check = read(fd, input, 100);
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
									check = read(fd, input, 100);
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
									check = read(fd, input, 100);
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
									check = read(fd, input, 100);
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
									check = read(fd, input, 100);
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
				else if(input[i] == 'P'){
					//plot 'h print

					i++;
					if(check==i){
						check = read(fd, input, 100);
		
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
							check = read(fd, input, 100);
			
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
								check = read(fd, input, 100);
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
								check = read(fd, input, 100);
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
									check = read(fd, input, 100);
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
										check = read(fd, input, 100);
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
										check = read(fd, input, 100);
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
				else if(input[i] == 'O'){
					for(k = 0; k<7; k++){
						i++;
						if(check==i){
							check = read(fd, input, 100);
			
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
							check = read(fd, input, 100);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}
					if(input[i] == 'S'){
						i++;
						if(check==i){
							check = read(fd, input, 100);
			
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
						if(input[i] == 'P'){
							i++;
							if(check==i){
								check = read(fd, input, 100);
				
								if(check<0){
									printf("Problem with read\n");
									close(fd);
									return(-1);
								}
								i = 0;
							}
							if(input[i] == 'D'){
								if_cholesky = 1;
							}
						}
					}
				}
			}
			else{
				input[i] = toupper(input[i]); // ta kanei ola kefalaia

				curr = (struct element *) malloc (sizeof(struct element));
				if(curr == NULL){
					printf("Error to malloc!\n");
					return(-1);
				}
				
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
					if(input[i] == 'V' || input[i] == 'L'){
						counter_m2++;
					}
					i++;
					if(check==i){
						check = read(fd, input, 100);
		
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
							check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
							check = read(fd, input, 100);
							if(check<0){
								printf("Problem with read\n");
								close(fd);
								return(-1);
							}
							i = 0;
						}
					}

					curr->value = atof(curr_value);
					curr->dc = 0;
					continue;


				}
				else if(input[i] == 'D' || input[i] == 'M' || input[i] == 'Q') {
					curr->type = input[i];
					i++;
					if(check==i){
						check = read(fd, input, 100);
		
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
							check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
								check = read(fd, input, 100);
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
								check = read(fd, input, 100);
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
									check = read(fd, input, 100);
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
									check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
							check = read(fd, input, 100);
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
								check = read(fd, input, 100);
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
								check = read(fd, input, 100);
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
								check = read(fd, input, 100);
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
								check = read(fd, input, 100);
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
								check = read(fd, input, 100);
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
								check = read(fd, input, 100);
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

			printf("%d: %c%s %lu %lu %.*lf \n",i, curr->type, curr->name, find_index(curr->pos), find_index(curr->neg), PREC,curr->value);
			
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

				//solve the system Ax = b
				if(if_cholesky == 0){
					LU_analysis(nodes(),m2_elem());
				} else {
					check = Cholesky_analysis(nodes(),m2_elem());
					if(check == -1){
						return -1;
					}
				}

				//find the node you want to print
				for(k=0; k<(nodes()-1); k++){
					if(gsl_vector_get(x_help,k) == if_print[i]){

						//write the results to file
						sprintf(curr_write,"%lf", j);
						check = write(fd, curr_write, strlen(curr_write));
						if(check<0){
							perror("write");
							return -1;
						}
						for(check = 0; check < 20; check++){
							curr_write[check] = '\0';
						}
						check = write(fd, "\t" , strlen(" "));
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
			if(curr->type == 'V'){
				MNA_voltage_dc(curr, curr->value, nodes());
			}
			else if(curr->type == 'I'){
				MNA_power_dc(curr, curr->value, curr->end_value);
			}
			close(fd);
		}
	}
	return 0;
}