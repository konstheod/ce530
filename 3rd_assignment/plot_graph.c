#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h> // for open read write
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define NUM_POINTS 5
#define NUM_COMMANDS 2


#define LINE_SIZE 200

int main(int argc, char *argv[]){

	FILE *pipe = NULL;
	FILE * temp1 = NULL;
	FILE * temp2 = NULL;
	FILE * temp3 = NULL;

	int fd,check, i, j;
	char file_name[50];
	char number[20] = {'\0'};
	char curr;
	int run = 1;
	char line[3000];

	double value;
	int values_counter = 3; //counts how many voltages we have to plot addint to that the y values

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

	pipe = popen( "gnuplot -persist", "w" );
	if(pipe == NULL){
		printf("Failed to open the Pipe\n");
		close(fd);
	}

	
	temp1 = fopen("data1.temp", "w");
	temp2 = fopen("data2.temp", "w");
	temp3 = fopen("data3.temp", "w");
	char * commandsForGnuplot[] = {"set title \"TITLEEEEE\"", "plot 'data.temp'"};

	j = 0;
	int counter = 0;
	while(run){
		counter ++;
		i = 0;
		while(1){
			check = read(fd, &curr, 1);
			if(check<0){
				printf("Problem with read\n");
				close(fd);
				return(-1);
			}
			else if(check==0){
				run = 0;
				break;
			}
			if(isblank(curr)){
				break;
			}else{
				number[i] = curr;
				i++;
			}
		}
		value = atof(number);
		
		if(j==0){
			fprintf(temp1, "%.14lf ",value); //Write the data to a temporary file
			fprintf(temp2, "%.14lf ",value);
			fprintf(temp3, "%.14lf ",value);
			j++;
		}else if(j==1){
			fprintf(temp1, "%.14lf \n",value);
    		j++;
		}else if(j==2){
			fprintf(temp2, "%.14lf \n",value);
			j++;
		}else{
			fprintf(temp3, "%.14lf \n",value);
			j = 0;
		}
	}

	//fprintf(temp, "\n");
	fprintf(pipe, "%s \n", "set title 'Cavendish Data'");
	fprintf(pipe, "%s \n", "set xlabel 'Steps'");
	fprintf(pipe, "%s \n", "set ylabel 'Voltage'");
	fprintf(pipe, "%s \n", "set grid");
	fprintf(pipe, "%s \n", "set zeroaxis");
	
	fprintf(pipe, "%s \n", "set key top right");
	fprintf(pipe, "%s \n", "set terminal wxt size 800,600");
	fprintf(pipe, "%s \n", "set autoscale");

	line[0] = '\0';
	sprintf(line, "plot \'%s\' using 1:2  title \'%s\' with line", "data1.temp", "Flow1");
	fprintf(pipe, "%s \n",line);

	line[0] = '\0';
	sprintf(line, "replot \'%s\' using 1:2  title \'%s\' with line", "data2.temp", "Flow2");
	fprintf(pipe, "%s \n",line);

	line[0] = '\0';
	sprintf(line, "replot \'%s\' using 1:2  title \'%s\' with line", "data3.temp", "Flow3");
	fprintf(pipe, "%s \n",line);
	
	//fprintf(pipe, "%s \n", "plot 'data1.temp' using 1:2  title 'Flow1' with line");

	//fprintf(pipe, "%s \n", "replot 'data2.temp' using 1:2  title 'Flow2' with lines");
	//fprintf(pipe, "%s \n", "replot");

	//fprintf(pipe, "%s \n", "replot 'data3.temp' using 1:2  title 'Flow3' with lines");
	//fprintf(pipe, "%s \n", "replot");




	printf("FINISHED %d!\n", counter);
	close(fd);
	fclose(pipe);
	fclose(temp1);
	fclose(temp2);
	fclose(temp3);
	remove("data1.temp");
	remove("data2.temp");
	remove("data3.temp");
	return (0);
}








