#include "spice.h"
cs_di *sparse_MNA;
cs_di *C_sparse;
cs_di *C_sparse_curr;
cs_di *compressed_MNA;
cs_di *MNA_tran_sparse;
cs_di *mna_curr_sparse;
double *b_sparse;
double *x_sparse;
double *e_sparse;
double *e_prev_sparse;
double *b_curr_sparse;
unsigned long int *x_help;
int vol_counter_sparse = 0;
int non_zeros;

void MNA_init_sparse(int node_sum, int m2_elem){
    int i;
    //calloc ston pinaka mna    
    sparse_MNA = cs_di_spalloc( (node_sum+m2_elem-1), (node_sum+m2_elem-1), non_zeros, 1, 1);
    
    //calloc sto dianusma b
    b_sparse = (double *)malloc((node_sum+m2_elem-1)*sizeof(double));
    x_sparse = (double *)malloc((node_sum+m2_elem-1)*sizeof(double));

    for(i=0; i<(node_sum+m2_elem-1); i++){
       b_sparse[i] = 0.0;
       x_sparse[i] = 0.0;
    }

    x = gsl_vector_calloc ((node_sum+m2_elem-1));

    x_help = (unsigned long int *)malloc(sizeof(unsigned long int)*(node_sum+m2_elem-1));
}

int MNA_conductance_sparse(struct element *cont, int node_sum, int m2_elem){
    unsigned long int pos, neg;
    double value;
    
    //pairnoume thn timh ths adistashs kai th metatrepoume se agwgimothta gia na thn prosthesoume ston pinaka
    value = cont->value;
    if(value == 0){
        value = 0.0;
    }
    else {
        value = 1/value;
    }
    
    //pairnoume tous komvous tou stoixeiou mesa apo to hash_table wste na topothethsoume to value sth swsth thesh ston pinaka
    pos = find_index(cont->pos);
    neg = find_index(cont->neg);

    //an kapoios komvos einai geiwsh tote vazoume t value mono sto diagwnio
    if(pos == 0){
        cs_di_entry (sparse_MNA, neg-1, neg-1,  value);

        x_help[neg-1] = cont->neg;
        return(1);
    }

    if(neg == 0 ){
        cs_di_entry (sparse_MNA, pos-1, pos-1,  value);
        x_help[pos-1] = cont->pos;
        return(1);
    }

    //anathesh timwn ston pinaka otan kanenas komvos den einai geiwsh  
    cs_di_entry (sparse_MNA, pos-1, pos-1,  value);

    cs_di_entry (sparse_MNA, neg-1, neg-1,  value);
    cs_di_entry (sparse_MNA, pos-1, neg-1,   -value);
        
    cs_di_entry (sparse_MNA, neg-1, pos-1,   -value);

    x_help[neg-1] = cont->neg;    
    x_help[pos-1] = cont->pos;    

    return(0);
}

int MNA_power_sparse(struct element *power){
   unsigned long int pos, neg;
   double value;
    
    value = power->value;
    
    pos = find_index(power->pos);
    neg = find_index(power->neg); 
    
    if(pos == 0){
        b_sparse[neg-1] += value;

        x_help[neg-1] = power->neg;
        return 1;
    }
    if(neg == 0){
        b_sparse[pos-1] -= value;

        x_help[pos-1] = power->pos;
        return 1;
    }

    b_sparse[neg-1] += value;
    if(value != 0){
        b_sparse[pos-1] -= value;
    }


    x_help[neg-1] = power->neg;
    x_help[pos-1] = power->pos;

    return 0;

}

int MNA_power_dc_sparse(struct element *power, double value, double old_value){
    unsigned long int pos, neg;
    
    pos = find_index(power->pos);
    neg = find_index(power->neg); 
    
    if(pos == 0){
        b_sparse[neg-1] -= old_value;
        b_sparse[neg-1] += value;
        return 1;
    }
    if(neg == 0){
        b_sparse[pos-1] += old_value;
        b_sparse[pos-1] -= value;
        return 1;
    }
    
    b_sparse[neg-1] -= old_value;
    b_sparse[neg-1] += value;
    b_sparse[pos-1] += old_value;
    b_sparse[pos-1] -= value;

    return 0;
}


int MNA_voltage_sparse(struct element *vol, int node_sum, int m2_elem){
    unsigned long int pos, neg;
    double value;
    
    value = vol->value;

    
    //prosthiki sto dianusma b
    pos = find_index(vol->pos);
    neg = find_index(vol->neg); 
    
    if(vol->type == 'L'){
       value = 0.0;
    }
    if(value!=0.0){
        b_sparse[node_sum - 1 + vol_counter_sparse] += value;
    }

    x_help[node_sum - 1 + vol_counter_sparse] = 0;
    
    //prosthiki sto mna
    if(pos != 0){
        cs_di_entry (sparse_MNA, node_sum + vol_counter_sparse -1, pos-1,  1);
  
        cs_di_entry (sparse_MNA, pos-1, node_sum + vol_counter_sparse -1,   1);

        x_help[pos-1] = vol->pos;
    }
    if(neg != 0){
        cs_di_entry (sparse_MNA, node_sum + vol_counter_sparse -1, neg-1,   - 1);
        
        cs_di_entry (sparse_MNA, neg-1, node_sum + vol_counter_sparse -1,   - 1);

        x_help[neg-1] = vol->neg;
    }
    vol->b_position = vol_counter_sparse;
    vol_counter_sparse++; 
    return 0;
}

int MNA_voltage_dc_sparse(struct element *vol,double value, int node_sum){

    //prosthiki sto dianusma b
    b_sparse[node_sum - 1 + vol->b_position] = value;
    
    return 0;
}

int C_inductor_sparse(struct element *ind, int node_sum){
    double value;
    
    value = ind->value;
    if(if_BE){
        value = value/time_step;
    }
    else{
        value = (2*value)/time_step;    
    }
    cs_di_entry(C_sparse_curr, (node_sum + vol_counter_sparse - 2) , (node_sum + vol_counter_sparse - 2),  -value);
    vol_counter_sparse++;
    return 0;
}

int C_capacity_sparse(struct element *cap){
    unsigned long int pos, neg;
    double value;
    
    //pairnoume thn timh ths adistashs kai th metatrepoume se agwgimothta gia na thn prosthesoume ston pinaka
    value = cap->value;
    if(if_BE){
        value = value/time_step;
    }
    else{
        value = (2*value)/time_step;    
    }
    
    //pairnoume tous komvous tou stoixeiou mesa apo to hash_table wste na topothethsoume to value sth swsth thesh ston pinaka
    pos = find_index(cap->pos);
    neg = find_index(cap->neg);
    
    //an kapoios komvos einai geiwsh tote vazoume t value mono sto diagwnio
    if(pos == 0){
        cs_di_entry(C_sparse_curr, neg-1, neg-1,  value);
        return(1);
    }
    if(neg == 0){
        cs_di_entry(C_sparse_curr, (pos-1), (pos-1), value);
        return(1);
    }
    
    //anathesh timwn ston pinaka otan kanenas komvos den einai geiwsh
    cs_di_entry(C_sparse_curr, (neg-1), (neg-1), value);
    cs_di_entry(C_sparse_curr, (pos-1), (pos-1), value);
    cs_di_entry(C_sparse_curr, (pos-1), (neg-1), -value);
    cs_di_entry(C_sparse_curr, (neg-1), (pos-1), -value);

    return(0);
}

int MNA_power_tran_sparse(struct element *power, double timestamp) {
    unsigned long int pos, neg;
    double value;
    double old_value;

    if(power->transient_spec == EXP) {
        value = calc_exp(power->exp_spec, timestamp);
        if(timestamp!=time_step) {
            old_value = calc_exp(power->exp_spec, timestamp - time_step);
        }
    }
    else if(power->transient_spec == SIN) {
        value = calc_sin(power->sin_spec, timestamp);
        if(timestamp!=time_step) {
            old_value = calc_sin(power->sin_spec, timestamp - time_step);
        }
    }
    else if(power->transient_spec == PULSE) {
        
        value = calc_pulse(power->pulse_spec, timestamp);
        if(timestamp!=time_step) {
            old_value = power->pulse_spec->old_value;
        }       

        power->pulse_spec->old_value = value;
    }
    else if(power->transient_spec == PWL) {
        value = calc_pwl(power, timestamp);
        if(timestamp!=time_step) {
            old_value = calc_pwl(power, timestamp - time_step);
        }
    }

    if(timestamp == time_step) {
        old_value = power->value;
    }

    
    
    pos = find_index(power->pos);
    neg = find_index(power->neg); 
    
    if(pos == 0){
        b_sparse[neg-1] = b_sparse[neg-1] - old_value;
        b_sparse[neg-1] = b_sparse[neg-1] + value;
        return 1;
    }
    if(neg == 0){
        b_sparse[pos-1] = b_sparse[pos-1]  + old_value;
        b_sparse[pos-1] = b_sparse[pos-1]  - value;
        return 1;
    }

    b_sparse[neg-1] = b_sparse[neg-1] - old_value;
    b_sparse[neg-1] = b_sparse[neg-1] + value;
    b_sparse[pos-1] = b_sparse[pos-1]  + old_value;
    b_sparse[pos-1] = b_sparse[pos-1]  - value;

    return 0;
}
int MNA_voltage_tran_sparse(struct element *vol, int node_sum, double timestamp) {
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

    b_sparse[node_sum - 1 + vol->b_position] = value;

    return 0;
}

void free_mna_sparse(){

    free(x_help);
    free(x_sparse);
    free(b_sparse);
    gsl_vector_free(x);

}

void print_MNA_sparse(int node_sum, int m2_elem){
    int i;

    printf("----MNA----\n");
    cs_di_print (sparse_MNA,0);

    printf("\n----b----\n");
    for(i=0; i<(node_sum+m2_elem-1); i++){
    
       printf("%lf \n",b_sparse[i]);
    
    }

    printf("\n----x_help----\n");
    for(i=0; i<(node_sum+m2_elem-1); i++){
    
       printf("%s \n",find_value(x_help[i]));
    
    }

    
    printf("\n");
}

void constructor_sparse(int node_sum, int m2_elem, struct element *head){
    MNA_init_sparse(node_sum, m2_elem);
    struct element *curr;
    int i = 0;

    for(curr = head; curr != NULL; curr = curr->next){
        if(curr->type == 'R'){
            MNA_conductance_sparse(curr, node_sum, m2_elem);
        }
        else if(curr->type == 'I') {
            MNA_power_sparse(curr);
        }
        else if(curr->type == 'V' || curr->type == 'L') {
            MNA_voltage_sparse(curr, node_sum, m2_elem);
        }
        i++;
    }
    compressed_MNA = cs_di_compress(sparse_MNA);
    //if(!if_tran) {
    cs_di_spfree(sparse_MNA);
    //}
    cs_di_dupl(compressed_MNA);
}


void sparse_matrix(int node_sum, int m2_elem){

    if(if_cholesky){
     printf("Computing x with Cholesky analysis \n");
     sparse_Cholesky_analysis(node_sum, m2_elem);
    }
    else if(if_CG){
     printf("Computing x with CG analysis \n");
     sparse_CG_analysis(node_sum, m2_elem);
        
    }
    else if(if_Bi_CG){
     printf("Computing x with Bi_CG analysis \n");
     sparse_Bi_CG_analysis(node_sum, m2_elem);
    }
    else{
     printf("Computing x with LU analysis \n");
     sparse_LU_analysis(node_sum, m2_elem);
    }

}

void constructor_C_sparse(int node_sum, int m2_elem, struct element *head){
    C_sparse_curr = cs_di_spalloc( (node_sum+m2_elem-1), (node_sum+m2_elem-1), non_zeros, 1, 1);
    C_sparse = cs_di_spalloc( (node_sum+m2_elem-1), (node_sum+m2_elem-1), non_zeros, 1, 1);
    mna_curr_sparse = cs_di_spalloc( (node_sum+m2_elem-1), (node_sum+m2_elem-1), non_zeros, 1, 1);
    MNA_tran_sparse = cs_di_spalloc( (node_sum+m2_elem-1), (node_sum+m2_elem-1), non_zeros, 1, 1);
    e_sparse = (double *)malloc((node_sum+m2_elem-1)*sizeof(double));
    e_prev_sparse = (double *)malloc((node_sum+m2_elem-1)*sizeof(double));
    b_curr_sparse = (double *)malloc((node_sum+m2_elem-1)*sizeof(double));


    struct element *curr;
    for(curr = head; curr != NULL; curr = curr->next){
        if(curr->type == 'C') {
            C_capacity_sparse(curr);
        }
        else if(curr->type == 'L') {
            C_inductor_sparse(curr, node_sum);
        }
    }
    C_sparse = cs_di_compress(C_sparse_curr);
    cs_di_spfree(C_sparse_curr);
    cs_di_dupl(C_sparse);
}

void constructor_tran_sparse(int node_sum, int m2_elem, struct element *head, double timestamp) {
    struct element *curr;
    for(curr = head; curr != NULL; curr = curr->next){
        if(curr->transient_spec != -1) {
            if(curr->type == 'I') {
                MNA_power_tran_sparse(curr, timestamp);

            }
            else if(curr->type == 'V') {
                MNA_voltage_tran_sparse(curr, node_sum, timestamp);

            }
        }
    }
}

void b_tran_constructor_sparse(int node_sum, int m2_elem, double timestamp) {
    int i,j,p;
    
    double *curr_vector = (double *)malloc((node_sum+m2_elem-1)*sizeof(double));
    double *temp = (double *)malloc((node_sum+m2_elem-1)*sizeof(double));
    
    cs_di *curr_matrix = cs_di_spalloc( (node_sum+m2_elem-1), (node_sum+m2_elem-1), non_zeros, 1, 1);
    cs_di *curr_matrix_mna = cs_di_spalloc( (node_sum+m2_elem-1), (node_sum+m2_elem-1), non_zeros, 1, 1);

    //memcopy_compressed_matrices(curr_matrix_mna, mna_curr_sparse, node_sum, m2_elem); 
    // cs_di_spfree( curr_matrix_mna );
    // curr_matrix_mna = mna_curr_sparse;
    curr_matrix_mna = memcopy_compressed_matrices(curr_matrix_mna, mna_curr_sparse, node_sum, m2_elem);

    //printf("----MNA----\n");
    //cs_di_print (mna_curr_sparse,0);

    if(timestamp != 0) {
        if(!if_BE) {
            curr_matrix_mna = cs_di_add(curr_matrix_mna,C_sparse, 1.0, -1.0);
            // printf("----C_sparse----\n");
            // cs_di_print (curr_matrix_mna,0);
            // cs_di_add( G2, C2, 1.0, 1.0 );

            memcpy(temp, e_sparse, (node_sum+m2_elem-1)*sizeof(double));

            for(i = 0; i<(node_sum+m2_elem-1); i++) {
                e_sparse[i] += e_prev_sparse[i];
            }
            // mul_vector_matrix_sparse(curr_matrix_mna, x_sparse, curr_vector, node_sum, m2_elem);
            for(j = 0; j < (node_sum+m2_elem-1); j++ ){
                curr_vector[j] = 0.0;
                for(p = curr_matrix_mna->p[j]; p < curr_matrix_mna->p[j+1]; p++ ){
                    curr_vector[j] = curr_vector[j] - curr_matrix_mna->x[p]*x_sparse[curr_matrix_mna->i[p]];
                }
            }

            for(i = 0; i<(node_sum+m2_elem-1); i++) {
                e_sparse[i] -= curr_vector[i];
            }

            memcpy(b_sparse, e_sparse, sizeof(double)*(node_sum+m2_elem-1));
            memcpy(e_prev_sparse, temp, sizeof(double)*(node_sum+m2_elem-1));
        }
            

            // mul_vector_matrix(curr_vector, x, 1, curr_matrix_mna); 
            
            

        else {
            mul_vector_matrix_sparse(C_sparse, x_sparse, curr_vector, node_sum, m2_elem);

            for(i=0; i<(node_sum+m2_elem-1); i++){
                    curr_vector[i] *= 1/time_step;
            }
            for(i=0; i<(node_sum+m2_elem-1); i++){
                    e_sparse[i] += curr_vector[i];
            }
            memcpy(b_sparse, e_sparse, sizeof(double)*(node_sum+m2_elem-1));
        }
    }

    free(curr_vector);
    free(temp);
    cs_di_spfree(curr_matrix);
    cs_di_spfree(curr_matrix_mna);
}

void sparse_set_x(int node_sum, int m2_elem){
    int i;

    for(i = 0; i < (node_sum - 1 + m2_elem); i++){

            gsl_vector_set(x, i, x_sparse[i]);
    }
}
