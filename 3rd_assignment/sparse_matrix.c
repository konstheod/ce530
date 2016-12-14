#include "spice.h"
cs_di *sparse_MNA;
double *b_sparse;
double *x_sparse;
unsigned long int *x_help;
int vol_counter_sparse = 0;
int non_zeros;
cs_di *compressed_MNA;

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
    cs_di_spfree(sparse_MNA);
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

void sparse_set_x(int node_sum, int m2_elem){
    int i;

    for(i = 0; i < (node_sum - 1 + m2_elem); i++){

            gsl_vector_set(x, i, x_sparse[i]);
    }
}