#include "spice.h"

void sparse_matrix(int node_sum, int m2_elem){
    cs_di *sparse_MNA, *compressed_MNA;
    int len  = node_sum -1 + m2_elem;
    int i, j;
    int non_zer = len*len;
    double value;
    
    for(i = 0; i < len; i++){
        for(j = 0; j < len; j++){
            if(gsl_matrix_get(mna,i,j) == 0){
                non_zer--;
            }
        }
    }

    
    sparse_MNA = cs_di_spalloc(len, len, non_zer, 1, 1);

    for(i = 0; i < len; i++){
        for(j = 0; j < len; j++){
            value = gsl_matrix_get(mna,i,j);
            if(value != 0.0){
                cs_di_entry (sparse_MNA, i, j, value);
            }
        }
    }
    sparse_MNA->nz = non_zer;

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

void sparse_set_x(double *x_sparse, int node_sum, int m2_elem){
    int i;
    for(i = 0; i < (node_sum - 1 + m2_elem); i++){
        gsl_vector_set(x, i, x_sparse[i]);
    }
}