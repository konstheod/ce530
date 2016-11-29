#include "spice.h"
cs_di *sparse_MNA;
double *b_sparse;
double *x_sparse;
unsigned long int *x_help;
int vol_counter_sparse = 0;

void MNA_init_sparse(int node_sum, int m2_elem){
    int i,j;
    //calloc ston pinaka mna    
    sparse_MNA = cs_di_spalloc( (node_sum+m2_elem-1), (node_sum+m2_elem-1), (node_sum+m2_elem-1)*(node_sum+m2_elem-1), 1, 1);
    for(i=0; i<(node_sum+m2_elem-1); i++){
        for(j=0; j<(node_sum+m2_elem-1); j++){
            cs_di_entry (sparse_MNA, i, j, 0.0);
        }

    }
    //calloc sto dianusma b
    b_sparse = (double *)calloc((node_sum+m2_elem-1), sizeof(double));
    x_sparse = (double *)calloc((node_sum+m2_elem-1), sizeof(double));

    x_help = (unsigned long int *)malloc(sizeof(unsigned long int)*(node_sum+m2_elem-1));
}

int MNA_conductance_sparse(struct element *cont, int node_sum, int m2_elem){
    unsigned long int pos, neg, position;
    double value;
    
    //pairnoume thn timh ths adistashs kai th metatrepoume se agwgimothta gia na thn prosthesoume ston pinaka
    value = cont->value;
    value = 1/value;
    
    //pairnoume tous komvous tou stoixeiou mesa apo to hash_table wste na topothethsoume to value sth swsth thesh ston pinaka
    pos = find_index(cont->pos);
    neg = find_index(cont->neg);
    
    //an kapoios komvos einai geiwsh tote vazoume t value mono sto diagwnio
    if(pos == 0){
        position = (neg-1)*(node_sum+m2_elem-1) + (neg-1);
        sparse_MNA->x[position] += value;

        //gsl_matrix_set (mna, (neg-1), (neg-1), gsl_matrix_get(mna, (neg-1), (neg-1)) + value);
        x_help[neg-1] = cont->neg;
        return(1);
    }
    if(neg == 0){
        position = (pos-1)*(node_sum+m2_elem-1) + (pos-1);
        sparse_MNA->x[position] += value;
        // gsl_matrix_set (mna, (pos-1), (pos-1), gsl_matrix_get(mna, (pos-1), (pos-1)) + value);
        x_help[pos-1] = cont->pos;
        return(1);
    }
    
    //anathesh timwn ston pinaka otan kanenas komvos den einai geiwsh
    position = (neg-1)*(node_sum+m2_elem-1) + (neg-1);
    sparse_MNA->x[position] += value;
    // gsl_matrix_set (mna, (neg-1), (neg-1), gsl_matrix_get(mna, (neg-1), (neg-1)) + value);

    position = (pos-1)*(node_sum+m2_elem-1) + (pos-1);
    sparse_MNA->x[position] += value;
    // gsl_matrix_set (mna, (pos-1), (pos-1), gsl_matrix_get(mna, (pos-1), (pos-1)) + value);


    position = (pos-1)*(node_sum+m2_elem-1) + (neg-1);
    sparse_MNA->x[position] -= value;
    //gsl_matrix_set (mna, (pos-1), (neg-1), gsl_matrix_get(mna, (pos-1), (neg-1)) - value);
    
    position = (neg-1)*(node_sum+m2_elem-1) + (pos-1);
    sparse_MNA->x[position] -= value;
    //gsl_matrix_set (mna, (neg-1), (pos-1), gsl_matrix_get(mna, (neg-1), (pos-1)) - value);

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

        //gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) + value);
        x_help[neg-1] = power->neg;
        return 1;
    }
    if(neg == 0){
        b_sparse[pos-1] -= value;

        //gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) - value);
        x_help[pos-1] = power->pos;
        return 1;
    }

    b_sparse[neg-1] += value;
    // gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) + value);
    
    b_sparse[pos-1] -= value;
    // gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) - value);

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
        // gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) - old_value);
        b_sparse[neg-1] += value;
        // gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) + value);
        return 1;
    }
    if(neg == 0){
        b_sparse[pos-1] += old_value;
        //gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) + old_value);
        b_sparse[pos-1] -= value;
        //gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) - value);
        return 1;
    }
        // printf("MNA_power_dc %lf\n", gsl_vector_get(b,(neg-1)));
    
    b_sparse[neg-1] -= old_value;
    //gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) - old_value);
    b_sparse[neg-1] += value;
    //gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) + value);
    b_sparse[pos-1] += old_value;
    //gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) + old_value);
    b_sparse[pos-1] -= value;
    //gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) - value);

    return 0;
}


int MNA_voltage_sparse(struct element *vol, int node_sum, int m2_elem){
    unsigned long int pos, neg, position;
    double value;
    
    value = vol->value;
    
    //prosthiki sto dianusma b
    pos = find_index(vol->pos);
    neg = find_index(vol->neg); 
    
    if(vol->type == 'L'){
       value = 0;
    }
    
    b_sparse[node_sum - 1 + vol_counter_sparse] += value;
    //gsl_vector_set(b,(node_sum - 1 + vol_counter_sparse),value);
    x_help[node_sum - 1 + vol_counter_sparse] = 0;
    
    
    //prosthiki sto mna
    if(pos != 0){
        position = (node_sum + vol_counter_sparse -1)*(node_sum+m2_elem-1) + (pos-1);
        sparse_MNA->x[position] += 1;

        // gsl_matrix_set (mna, (node_sum + vol_counter_sparse -1), (pos-1), gsl_matrix_get(mna, (node_sum + vol_counter_sparse -1), (pos-1)) + 1);
  
        position = (pos-1)*(node_sum+m2_elem-1) + (node_sum + vol_counter_sparse -1 );
        sparse_MNA->x[position] += 1;

        //gsl_matrix_set (mna, (pos-1), (node_sum + vol_counter_sparse -1), gsl_matrix_get(mna, (pos-1), (node_sum + vol_counter_sparse -1 )) + 1);
        x_help[pos-1] = vol->pos;
    }
    if(neg != 0){
        position = (node_sum + vol_counter_sparse -1)*(node_sum+m2_elem-1) + (neg-1);
        sparse_MNA->x[position] -= 1;

        //gsl_matrix_set (mna, (node_sum + vol_counter_sparse -1), (neg-1), gsl_matrix_get(mna, (node_sum + vol_counter_sparse -1), (neg-1)) - 1);
        
        position = (neg-1)*(node_sum+m2_elem-1) + (node_sum + vol_counter_sparse -1);
        sparse_MNA->x[position] -= 1;

        // gsl_matrix_set (mna, (neg-1), (node_sum + vol_counter_sparse -1), gsl_matrix_get(mna, (neg-1), (node_sum + vol_counter_sparse -1)) - 1);
        x_help[neg-1] = vol->neg;
    }
    vol->b_position = vol_counter_sparse;
    vol_counter_sparse++; 
    return 0;
}

int MNA_voltage_dc_sparse(struct element *vol,double value, int node_sum){

    //prosthiki sto dianusma b
    b_sparse[node_sum - 1 + vol->b_position] = value;
    // gsl_vector_set(b,(node_sum - 1 + vol->b_position),value);
    
    return 0;
}

void free_mna_sparse(){

    free(x_help);
    free(x_sparse);
    free(b_sparse);

    // gsl_vector_free (x);
    // gsl_vector_free(b);
    // gsl_matrix_free(mna);
}

void print_MNA_sparse(int node_sum, int m2_elem){
    int i,j;
    unsigned long int  position;

    printf("----MNA----\n");
    for(i=0; i<(node_sum+m2_elem-1); i++){
        for(j=0; j<(node_sum+m2_elem-1); j++){
            position = i*(node_sum+m2_elem-1) + j;
            printf("%lf  ",sparse_MNA->x[position]);
        }
        printf("\n");
    }

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
    }
}





void sparse_matrix(int node_sum, int m2_elem){
    cs_di *compressed_MNA;

 

    compressed_MNA = cs_di_compress(sparse_MNA);
     

    cs_di_spfree(sparse_MNA);
    cs_di_dupl(compressed_MNA);
   	// cs_di_print (compressed_MNA,0);
    if(if_cholesky){
		printf("Computing x with Cholesky analysis \n");
		sparse_Cholesky_analysis(compressed_MNA, node_sum, m2_elem);
	}
	else if(if_CG){
		printf("Computing x with CG analysis \n");
		sparse_CG_analysis(compressed_MNA, node_sum, m2_elem);
		
	}
	else if(if_Bi_CG){
		printf("Computing x with Bi_CG analysis \n");
		sparse_Bi_CG_analysis(compressed_MNA, node_sum, m2_elem);
	}
	else{
		printf("Computing x with LU analysis \n");
    	sparse_LU_analysis(compressed_MNA, node_sum, m2_elem);
	}

    cs_di_spfree(compressed_MNA);
}

void sparse_set_x(int node_sum, int m2_elem){
    int i;
    for(i = 0; i < (node_sum - 1 + m2_elem); i++){
        gsl_vector_set(x, i, x_sparse[i]);
    }
}