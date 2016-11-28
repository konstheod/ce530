#ifndef _SPICE_
  
#define _SPICE_

#include <stdio.h>
#include <fcntl.h> // for open read write
#include <unistd.h>
#include <ctype.h> //for toupper, tolower, isspace
#include <string.h>
#include <stdlib.h> // for malloc free  
#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>

#include "Sparse/CXSparse/Include/cs.h"

#define NODE_SIZE 40
#define HASH_TABLE_SIZE 4

#define SIZE_VALUE 20
#define SIZE_NAME 20
#define NODE_SIZE 40
#define PREC 15
#define EPS 1e-16


struct node{
	char name[NODE_SIZE];
	unsigned long int index; //name to integer
	unsigned long int key; //hash key
	struct node *next;
};


struct element{
	char type; 
	unsigned long int pos;
	unsigned long int neg;
	double value;
	long int D; //for transistor
	long int G; //for transistor
	long int S; //for transistor
	long int B; //for transistor
	long int C; //for transistor
	long int E; //for transistor
	double L; //for transistor
	double W; //for transistor
	char model_name[SIZE_NAME]; //for transistor
	double area; //for transistor
	char name[SIZE_NAME];
	struct element *next;
	double start_value; //for .DC
	double end_value; //for .DC
	double increment; //for .DC
	int dc; //for .DC
	int b_position; //for .DC, the position of the element to vector b
};

extern struct node * hash_table[HASH_TABLE_SIZE];
extern gsl_matrix *mna;
extern gsl_vector *b;
extern gsl_vector *x_help;
extern gsl_vector *x;

int if_cholesky; //if user wants Cholesky analysis the variable turns to 1, else is set to 0
int if_Bi_CG;
int	if_CG;
double itol;
int if_sparse;
unsigned long int *if_print; //apothhkeuodai oi metavlhtes pou einai na ginoun PLOT/PRINT

/*creates hash code for a node*/
unsigned long int hash(char *name);
/*finds hash code(key) from a node name*/
unsigned long int find_node(char *name);
/*finds node name from hash code(key)*/
char* find_value( unsigned long int key);
/*finds node index from hash code(key)*/
unsigned long int find_index( unsigned long int key);
/*adds node to hash_table*/
unsigned long int add_node(char *name);
/*free hash table*/
int free_nodes(struct node * hash_table[]);
/*return the number of nodes*/
int nodes(void);

/*initializes MNA,b,x_help,x tables*/
void MNA_init(int node_sum, int m2_elem);
/*adds conductance elements to mna*/
int MNA_conductance(struct element *cont, int node_sum, int m2_elem);
/*adds power sources elements to mna*/
int MNA_power(struct element *power);
/*adds voltage sources elements to mna*/
int MNA_voltage(struct element *vol, int node_sum, int m2_elem);
/*adds voltage sources with dc specs elements to mna*/
int MNA_voltage_dc(struct element *vol,double value, int node_sum);
/*adds power sources with dc specs elements to mna*/
int MNA_power_dc(struct element *power, double value, double old_value);
/*free mna,b,x,x_help tables*/
void free_mna(void);
/*prints MNA matrix and b vector*/
void print_MNA(int node_sum, int m2_elem);
/*constructs MNA,b,x_help,x tables*/
void constructor(int node_sum, int m2_elem, struct element *head);

/*parses the netlist file and puts the elements to a list*/
int parser(struct element **element_head, int fd);
/*free the element list*/
int free_elements(struct element **element_head);
/*prints element list*/
void printList(struct element *head);
/*returns number of team 2 elements*/
int m2_elem(void);

/*solve Ax = b with LU*/
int LU_analysis(int node, int m2_elem);
/*solve Ax = b with Cholesky*/
int Cholesky_analysis(int node_sum, int m2_elem);

/*solve Ax = b with CG_analysis*/
int CG_analysis(int node_sum, int m2_elem);
/*solve Ax = b with Bi_CG_analysis*/
int Bi_CG_analysis(int node_sum, int m2_elem);

/*checking if the matrix MNA is SPD*/
int if_SPD(int node_sum, int m2_elem);
/*solve Mz = r (preconditioner)*/
void precondition_solver(gsl_vector *r, gsl_vector *z, gsl_vector *MNA_diag, int node_sum,int m2_elem);
double inner_product(gsl_vector *r, gsl_vector *z, int node_sum, int m2_elem);
void mul_vector_matrix(gsl_vector *q, gsl_vector *p, int check, gsl_matrix *mnaT);
void axpy_solve(double alpha, gsl_vector *x, gsl_vector *y, int node_sum, int m2_elem);
void get_diag_matrix(gsl_vector *MNA_diag, int node_sum, int m2_elem, int check, gsl_matrix *mnaT);
void matrix_transpose(gsl_matrix *dest, gsl_matrix *src, int node_sum, int m2_elem);

void get_b_sparse(double *b_sparse, int node_sum, int m2_elem);
void get_diag_matrix_sparse(cs_di *compressed_MNA, double *MNA_diag, int node_sum, int m2_elem);
double norm_sparse(double *sparse, int node_sum, int m2_elem);
void precondition_solver_sparse(double *r, double *z, double *MNA_diag, int node_sum, int m2_elem);
double inner_product_sparse(double *r, double *z, int node_sum, int m2_elem);
void mul_vector_matrix_sparse(cs_di *compressed_MNA, double *x, double *y, int node_sum, int m2_elem);
void axpy_solve_sparse(double alpha, double *x, double *y, int node_sum, int m2_elem);

void sparse_matrix(int node_sum, int m2_elem);
void sparse_LU_analysis(cs_di *compressed_MNA, int node_sum, int m2_elem);
void sparse_Cholesky_analysis();
void sparse_CG_analysis();
void sparse_Bi_CG_analysis();

/*prints to file the result of print*/
int plot(struct element *head);

/*prints the value of x without DC*/
void print_x(void);



#endif