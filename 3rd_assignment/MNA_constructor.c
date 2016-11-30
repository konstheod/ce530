#include "spice.h"


gsl_matrix *mna;
gsl_vector *b;
unsigned long int *x_help;
gsl_vector *x;

int vol_counter = 0;

void MNA_init(int node_sum, int m2_elem){

	//calloc ston pinaka mna	
    mna = gsl_matrix_calloc ((node_sum+m2_elem-1), (node_sum+m2_elem-1));

    //calloc sto dianusma b
    b = gsl_vector_calloc (node_sum+m2_elem-1);

    x_help = (unsigned long int *)malloc(sizeof(unsigned long int)*(node_sum+m2_elem-1));

    x = gsl_vector_calloc(node_sum+m2_elem-1);


}

int MNA_conductance(struct element *cont, int node_sum, int m2_elem){
    unsigned long int pos, neg;
    double value;
    
    //pairnoume thn timh ths adistashs kai th metatrepoume se agwgimothta gia na thn prosthesoume ston pinaka
    value = cont->value;
    value = 1/value;
    
    //pairnoume tous komvous tou stoixeiou mesa apo to hash_table wste na topothethsoume to value sth swsth thesh ston pinaka
    pos = find_index(cont->pos);
    neg = find_index(cont->neg);
    
    //an kapoios komvos einai geiwsh tote vazoume t value mono sto diagwnio
    if(pos == 0){
        gsl_matrix_set (mna, (neg-1), (neg-1), gsl_matrix_get(mna, (neg-1), (neg-1)) + value);
        x_help[neg-1] = cont->neg;
    	return(1);
    }
    if(neg == 0){
        gsl_matrix_set (mna, (pos-1), (pos-1), gsl_matrix_get(mna, (pos-1), (pos-1)) + value);
        x_help[pos-1] = cont->pos;
    	return(1);
    }
    
    //anathesh timwn ston pinaka otan kanenas komvos den einai geiwsh
    gsl_matrix_set (mna, (neg-1), (neg-1), gsl_matrix_get(mna, (neg-1), (neg-1)) + value);
    gsl_matrix_set (mna, (pos-1), (pos-1), gsl_matrix_get(mna, (pos-1), (pos-1)) + value);

    gsl_matrix_set (mna, (pos-1), (neg-1), gsl_matrix_get(mna, (pos-1), (neg-1)) - value);
    gsl_matrix_set (mna, (neg-1), (pos-1), gsl_matrix_get(mna, (neg-1), (pos-1)) - value);

    x_help[neg-1] = cont->neg;    
    x_help[pos-1] = cont->pos;    

    return(0);
}

int MNA_power(struct element *power){
   unsigned long int pos, neg;
   double value;
    
    value = power->value;
    
    pos = find_index(power->pos);
    neg = find_index(power->neg); 
    
    if(pos == 0){
        gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) + value);
        x_help[neg-1] = power->neg;
    	return 1;
    }
    if(neg == 0){
    	gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) - value);
        x_help[pos-1] = power->pos;
    	return 1;
    }
   
    gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) + value);
    gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) - value);

    x_help[neg-1] = power->neg;
    x_help[pos-1] = power->pos;

    return 0;

}

int MNA_power_dc(struct element *power, double value, double old_value){
    unsigned long int pos, neg;
    
    pos = find_index(power->pos);
    neg = find_index(power->neg); 
    
    if(pos == 0){
        gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) - old_value);
        gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) + value);
        return 1;
    }
    if(neg == 0){
        gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) + old_value);
        gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) - value);
        return 1;
    }
        // printf("MNA_power_dc %lf\n", gsl_vector_get(b,(neg-1)));
   
    gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) - old_value);
    gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) + value);
    gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) + old_value);
    gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) - value);

    return 0;
}


int MNA_voltage(struct element *vol, int node_sum, int m2_elem){
    unsigned long int pos, neg;
    double value;
    
    value = vol->value;
    
    //prosthiki sto dianusma b
    pos = find_index(vol->pos);
    neg = find_index(vol->neg); 
    
    if(vol->type == 'L'){
	   value = 0;
    }
    
    gsl_vector_set(b,(node_sum - 1 + vol_counter),value);
    x_help[node_sum - 1 + vol_counter] = 0;
    
    
    //prosthiki sto mna
    if(pos != 0){
        gsl_matrix_set (mna, (node_sum + vol_counter -1), (pos-1), gsl_matrix_get(mna, (node_sum + vol_counter -1), (pos-1)) + 1);
        gsl_matrix_set (mna, (pos-1), (node_sum + vol_counter -1), gsl_matrix_get(mna, (pos-1), (node_sum + vol_counter -1 )) + 1);
        x_help[pos-1] = vol->pos;
    }
    if(neg != 0){
        gsl_matrix_set (mna, (node_sum + vol_counter -1), (neg-1), gsl_matrix_get(mna, (node_sum + vol_counter -1), (neg-1)) - 1);
        gsl_matrix_set (mna, (neg-1), (node_sum + vol_counter -1), gsl_matrix_get(mna, (neg-1), (node_sum + vol_counter -1)) - 1);
        x_help[neg-1] = vol->neg;
    }
    vol->b_position = vol_counter;
    vol_counter++; 
    return 0;
}

int MNA_voltage_dc(struct element *vol,double value, int node_sum){

    //prosthiki sto dianusma b
    gsl_vector_set(b,(node_sum - 1 + vol->b_position),value);
    
    return 0;
}

void free_mna(){

    free(x_help);
    gsl_vector_free (x);
    gsl_vector_free(b);
    gsl_matrix_free(mna);
}

void print_MNA(int node_sum, int m2_elem){
    int i,j;
    
    printf("----MNA----\n");
    for(i=0; i<(node_sum+m2_elem-1); i++){
        for(j=0; j<(node_sum+m2_elem-1); j++){
            printf("%g  ",gsl_matrix_get(mna,i,j));
        }
        printf("\n");
    }
    printf("\n----b----\n");
    for(i=0; i<(node_sum+m2_elem-1); i++){
    
	   printf("%g \n",gsl_vector_get(b,i));
	
    }

    printf("\n----x_help----\n");
    for(i=0; i<(node_sum+m2_elem-1); i++){
    
       printf("%s \n",find_value(x_help[i]));
    
    }

    
    printf("\n");
}

void constructor(int node_sum, int m2_elem, struct element *head){
	MNA_init(node_sum, m2_elem);
	struct element *curr;
	for(curr = head; curr != NULL; curr = curr->next){
        if(curr->type == 'R'){
            MNA_conductance(curr, node_sum, m2_elem);
        }
        else if(curr->type == 'I') {
            MNA_power(curr);
        }
        else if(curr->type == 'V' || curr->type == 'L') {
            MNA_voltage(curr, node_sum, m2_elem);
        }
	}
}



