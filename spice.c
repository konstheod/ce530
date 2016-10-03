#include <stdio.h>
#include <fcntl.h> // for open read write
#include <unistd.h>
#include <ctype.h> //for toupper, tolower, isspace
#include <string.h>
#include <stdlib.h> // for malloc free 

#define SIZE_VALUE 20
#define SIZE_NAME 20
// int toupper(int c);
// int tolower(int c);

struct element{
	char type;
	int pos;
	int neg;
	double value;
	int D;
	int G;
	int S;
	int B;
	int C;
	int E;
	double L;
	double W;
	char model_name[SIZE_NAME];
	double area;
	char name[SIZE_NAME];


	struct element *next;
};




int main(int argc, char *argv[]){
	int fd, check;
	char file_name[50];
	struct element *head, *curr;


	//We ask for the file name of the circuit.
	printf("Give me the exact file name: ");
	scanf("%s",file_name );
	
	//strcpy(file_name, "circuit1.netlist");
	//strcpy(file_name, "circuit1_abnormal.netlist");
	strcpy(file_name, "transistor_circuit.netlist");
	//strcpy(file_name, "transistor_circuit_abnormal.netlist");

	printf("The file that you gave me is \"%s\"\n\n",file_name );
	
	//We open the file where the circuit is.
	fd = open(file_name, O_RDONLY);
	if(fd<0){
		printf("Error at the opening of the file.\n");
		return(-1);
	}

	int k, i, j=1, probe;
	char curr_value[SIZE_VALUE], input[100], curr_area[SIZE_VALUE];

	curr = (struct element *) malloc (sizeof(struct element));
	head = curr;
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
				//printf("\n");
				j++;
				i++;
				continue;
			}
			else if(input[i] == '*'){
				while(i<check && input[i]!='\n'){
					i++;
				}
				continue;
			}
			else if(input[i] == ' ' || input[i] == '\t'){
				i++;
				continue;
			}
			else{
				input[i] = toupper(input[i]); // ta kanei ola kefalaia
				printf("TO Upper %c", input[i]);
				if(input[i] == 'R' || input[i] == 'C' || input[i] == 'L' || input[i] == 'V' || input[i] == 'I'){
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

					//find positive probe
					while(input[i] != '\t' && input[i] != ' ') {

						//hashing

						curr->pos = atoi(&input[i]);
						printf(" %d",curr->pos);
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

					//find negative probe
					while(input[i] != '\t' && input[i] != ' ') {

						//hashing

						curr->neg = atoi(&input[i]);
						printf(" %d ",curr->neg);
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
					printf(" %lf \n",curr->value);
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
					while(input[i] != '\t' && input[i] != ' ') {

						//hashing

						probe = atoi(&input[i]);
						printf(" %d",probe);
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
						curr->pos = probe;
					}
					else if(curr->type == 'Q'){
						curr->C = probe;	
					}
					else{
						curr->D = probe;	
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
					while(input[i] != '\t' && input[i] != ' ') {

						//hashing

						probe = atoi(&input[i]);
						printf(" %d",probe);
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
						curr->neg = probe;
					}
					else if(curr->type == 'Q'){
						curr->B = probe;	
					}
					else{
						curr->G = probe;	
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
						while(input[i] != '\t' && input[i] != ' ') {

							//hashing

							probe = atoi(&input[i]);
							printf(" %d",probe);
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
							curr->E = probe;	
						}
						else{
							curr->S = probe;
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
							while(input[i] != '\t' && input[i] != ' ') {

								//hashing

								probe = atoi(&input[i]);
								printf(" %d",probe);
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

							curr->B = probe;
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

					printf(" %s", curr->model_name);
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
						printf(" %lf", curr->L);

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
						printf(" %lf", curr->W);
					}
					printf("\n");
				}
			}


			i++;
		}


		//End of file reached
		if(check < 100 ){
			break;
		}
	}

	printf("%d lines of code.\n",j);



	/*
	blueprint of the read function
	check = read(fd, &input[i], 1);
	if(check<0){
		printf("Problem with read\n");
		close(fd);
		return(-1);
	}*/


	close(fd);
	return(0);
}