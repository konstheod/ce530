#ifndef _SPICE_
  
#define _SPICE_

#include <stdio.h>
#include <fcntl.h> // for open read write
#include <unistd.h>
#include <ctype.h> //for toupper, tolower, isspace
#include <string.h>
#include <stdlib.h> // for malloc free 



#define NODE_SIZE 40
#define HASH_TABLE_SIZE 2000

#define SIZE_VALUE 20
#define SIZE_NAME 20
#define NODE_SIZE 40
#define PREC 15


struct node{
	char name[NODE_SIZE];
	unsigned long int index;
	unsigned long int key;
	struct node *next;
};


struct element{
	char type;
	long int pos;
	long int neg;
	double value;
	long int D;
	long int G;
	long int S;
	long int B;
	long int C;
	long int E;
	double L;
	double W;
	char model_name[SIZE_NAME];
	double area;
	char name[SIZE_NAME];
	struct element *next;
};

extern struct node * hash_table[HASH_TABLE_SIZE];
extern double *mna;
extern double *b;


unsigned int hash(char *name);
long int find_node(char *name);
char* find_value( long int key);
unsigned long int find_index( long int key);
long int add_node(char *name);
int free_nodes(struct node * hash_table[]);
int nodes(void);

void MNA_init(int node_sum, int m2_elem);
int MNA_conductance(struct element *cont, int node_sum, int m2_elem);
int MNA_power(struct element *power);
int MNA_voltage(struct element *vol, int node_sum, int m2_elem);
void free_mna(void);
void print_MNA(int node_sum, int m2_elem);
void constructor(int node_sum, int m2_elem, struct element *head);

int parser(struct element **element_head, int fd);
int free_elements(struct element **element_head);
void printList(struct element *head);
int m2_elem(void);


#endif