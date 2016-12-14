#include "spice.h"


gsl_matrix *mna;
gsl_matrix *mna_tran;
gsl_matrix *mna_curr;
gsl_matrix *C;
gsl_vector *b;
gsl_vector *e;
gsl_vector *e_prev;
gsl_vector *b_curr;
unsigned long int *x_help;
gsl_vector *x;

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

int MNA_power_tran(struct element *power, double timestamp){
    unsigned long int pos, neg;
    double value;
    double old_value;

    if(power->transient_spec == EXP) {
        value = calc_exp(power->exp_spec, timestamp);
    }
    else if(power->transient_spec == SIN) {
        value = calc_sin(power->sin_spec, timestamp);
    }
    else if(power->transient_spec == PULSE) {
        
        value = calc_pulse(power->pulse_spec, timestamp);
    }
    else if(power->transient_spec == PWL) {
        value = calc_pwl(power, timestamp);
    }

    old_value = power->value;


    
    
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
   
    gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) - old_value);
    gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) + value);
    gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) + old_value);
    gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) - value);

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

int MNA_voltage_tran(struct element *vol, int node_sum, double timestamp) {
    double value;

    if(vol->transient_spec == EXP) {
        value = calc_exp(vol->exp_spec, timestamp);
    }
    else if(vol->transient_spec == SIN) {
        value = calc_sin(vol->sin_spec, timestamp);
    }
    else if(vol->transient_spec == PULSE) {
        value = calc_pulse(vol->pulse_spec, timestamp);
    }
    else if(vol->transient_spec == PWL) {
        value = calc_pwl(vol, timestamp);
    }

    gsl_vector_set(b,(node_sum - 1 + vol->b_position),value);
    return 0;
}

int MNA_voltage_dc(struct element *vol,double value, int node_sum){

    //prosthiki sto dianusma b
    gsl_vector_set(b,(node_sum - 1 + vol->b_position),value);
    
    return 0;
}

int C_inductor(struct element *ind, int node_sum) {
    double value;
    
    value = ind->value;
    gsl_matrix_set (C, (node_sum + vol_counter -2), (node_sum + vol_counter -2), gsl_matrix_get(C, (node_sum + vol_counter - 2), (node_sum + vol_counter - 2)) - value);
    vol_counter++;
    return 0;
}

int C_capacity(struct element *cap) {
    unsigned long int pos, neg;
    double value;
    
    //pairnoume thn timh ths adistashs kai th metatrepoume se agwgimothta gia na thn prosthesoume ston pinaka
    value = cap->value;
    
    //pairnoume tous komvous tou stoixeiou mesa apo to hash_table wste na topothethsoume to value sth swsth thesh ston pinaka
    pos = find_index(cap->pos);
    neg = find_index(cap->neg);
    
    //an kapoios komvos einai geiwsh tote vazoume t value mono sto diagwnio
    if(pos == 0){
        gsl_matrix_set (C, (neg-1), (neg-1), gsl_matrix_get(C, (neg-1), (neg-1)) + value);
        return(1);
    }
    if(neg == 0){
        gsl_matrix_set (C, (pos-1), (pos-1), gsl_matrix_get(C, (pos-1), (pos-1)) + value);
        return(1);
    }
    
    //anathesh timwn ston pinaka otan kanenas komvos den einai geiwsh
    gsl_matrix_set (C, (neg-1), (neg-1), gsl_matrix_get(C, (neg-1), (neg-1)) + value);
    gsl_matrix_set (C, (pos-1), (pos-1), gsl_matrix_get(C, (pos-1), (pos-1)) + value);

    gsl_matrix_set (C, (pos-1), (neg-1), gsl_matrix_get(C, (pos-1), (neg-1)) - value);
    gsl_matrix_set (C, (neg-1), (pos-1), gsl_matrix_get(C, (neg-1), (pos-1)) - value);

    return(0);
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

    // printf("----C----\n");
    // for(i=0; i<(node_sum+m2_elem-1); i++){
    //     for(j=0; j<(node_sum+m2_elem-1); j++){
    //         printf("%g  ",gsl_matrix_get(C,i,j));
    //     }
    //     printf("\n");
    // }

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
    struct element *curr;

	MNA_init(node_sum, m2_elem);
    vol_counter = 0;
	
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

void constructor_tran(int node_sum, int m2_elem, struct element *head, double timestamp){
    struct element *curr;
    for(curr = head; curr != NULL; curr = curr->next){
        if(curr->transient_spec != -1) {
            if(curr->type == 'I') {
                MNA_power_tran(curr, timestamp);

            }
            else if(curr->type == 'V') {
                MNA_voltage_tran(curr, node_sum, timestamp);

            }
        }
    }
}

void constructor_tran_C(int node_sum, int m2_elem, struct element *head){
    C = gsl_matrix_calloc ((node_sum+m2_elem-1), (node_sum+m2_elem-1));
    mna_curr = gsl_matrix_calloc ((node_sum+m2_elem-1), (node_sum+m2_elem-1));
    mna_tran = gsl_matrix_calloc ((node_sum+m2_elem-1), (node_sum+m2_elem-1));
    e        = gsl_vector_calloc (node_sum+m2_elem-1);
    e_prev   = gsl_vector_calloc (node_sum+m2_elem-1);
    b_curr   = gsl_vector_calloc (node_sum+m2_elem-1);

    struct element *curr;
    for(curr = head; curr != NULL; curr = curr->next){
        if(curr->type == 'C') {
            C_capacity(curr);
        }
        else if(curr->type == 'L') {
            C_inductor(curr, node_sum);
        }
    }
}

void b_tran_constructor(int node_sum, int m2_elem, double timestamp) {
    gsl_vector *curr_vector = gsl_vector_calloc (node_sum+m2_elem-1);
    gsl_matrix *curr_matrix = gsl_matrix_calloc ((node_sum+m2_elem-1), (node_sum+m2_elem-1));
    gsl_matrix *curr_matrix_mna = gsl_matrix_calloc ((node_sum+m2_elem-1), (node_sum+m2_elem-1));

    gsl_vector *temp = gsl_vector_calloc (node_sum+m2_elem-1);

    int i, j;
    gsl_matrix_memcpy(curr_matrix_mna, mna_curr);
    if(timestamp != 0) {
        if(!if_BE) {
            printf("\nComputing with Trapezoidal\n");
            for(i=0; i<(node_sum+m2_elem-1); i++){
                for(j=0; j<(node_sum+m2_elem-1); j++){
                    gsl_matrix_set(curr_matrix, i, j, gsl_matrix_get(C, i, j)*(2/time_step));
                }
            }

            gsl_matrix_sub(curr_matrix_mna, curr_matrix);
            printf("----curr_matrix_mna----\n");
            for(i=0; i<(node_sum+m2_elem-1); i++){
                printf("col %d\n", i );
                for(j=0; j<(node_sum+m2_elem-1); j++){
                    if( gsl_matrix_get(curr_matrix_mna,i,j)!= 0.0){
                    printf("\t%d %g\n",j, gsl_matrix_get(curr_matrix_mna,i,j));
                }
                }
                printf("\n");
            }

            gsl_vector_memcpy(temp,e);

            gsl_vector_add(e, e_prev);
            mul_vector_matrix(curr_vector, x, 1, curr_matrix_mna); 
            gsl_vector_sub(e,curr_vector);
            gsl_vector_memcpy(b,e);
            gsl_vector_memcpy(e_prev, temp);
        }

        else {
            printf("\nComputing with Backward Euler\n");
            mul_vector_matrix(curr_vector, x, 1, C);
            for(i=0; i<(node_sum+m2_elem-1); i++){
                    gsl_vector_set(curr_vector, i, gsl_vector_get(curr_vector, i)*(1/time_step));
            }
            gsl_vector_add(e, curr_vector);
            gsl_vector_memcpy(b,e);
        }
    }

    gsl_vector_free(curr_vector);
    gsl_vector_free(temp);
    gsl_matrix_free(curr_matrix);
    gsl_matrix_free(curr_matrix_mna);
}





