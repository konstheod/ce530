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
	// else if(if_Bi_CG){
	// 	printf("Computing x with Bi_CG analysis \n");
	// 	check = Bi_CG_analysis(nodes(),m2_elem());
	// 	if(check == -1){
	// 		printf("Mulfunction with Bi_CG analysis \n");
	// 		return -1;
	// 	}
	// }
	else{
		printf("Computing x with LU analysis \n");
	 	// LU_analysis(nodes(),m2_elem());
    	sparse_LU_analysis(compressed_MNA, node_sum, m2_elem);
	}

    cs_di_spfree(compressed_MNA);
}



void sparse_CG_analysis(cs_di *compressed_MNA, int node_sum, int m2_elem){
	int i;
    int iter = 0;
    double norm_r, norm_b;
    double rho = 0;
    double beta = 0;
    double rho1 = 0;
    double alpha = 0;

    double *x_sparse = (double *) malloc((node_sum - 1 + m2_elem)*sizeof(double));
	double *b_sparse = (double *) malloc((node_sum - 1 + m2_elem)*sizeof(double));
	get_b_sparse(b_sparse, node_sum, m2_elem);

    double *r = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *z = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *p = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *q = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *MNA_diag = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    get_diag_matrix_sparse(compressed_MNA, MNA_diag, node_sum, m2_elem);
    printf("-----------------------MNA_DIAG_SPARSE-----------------------\n");
	for(i = 0; i < (node_sum - 1 + m2_elem); i++){
		printf("%lf\n", MNA_diag[i]);
	}

    for(i = 0; i<(node_sum+m2_elem-1); i++){
        x_sparse[i] = 0;
        q[i] = 0;
        r[i] = b_sparse[i];
    }

    norm_b = gsl_blas_dnrm2(b);
    if(fabs(norm_b) <= EPS){
        norm_b = 1;
    }
    while(1){

        norm_r = norm_sparse(r, node_sum, m2_elem);
        if((norm_r/norm_b) <= itol || iter >= (node_sum+m2_elem)*2){
            break;
        }

        iter++;

        precondition_solver_sparse(r, z, MNA_diag, node_sum, m2_elem);

        rho = inner_product_sparse(r, z, node_sum, m2_elem);

        if(iter == 1) {
        	for(i = 0; i<(node_sum+m2_elem-1); i++){
        		p[i] = z[i];
        	}
        }
        else {
            beta = rho/rho1;
            for(i = 0; i<(node_sum+m2_elem-1); i++){
        		p[i] = z[i] + beta*p[i];
        	}
        }

        rho1 = rho;
        mul_vector_matrix_sparse(compressed_MNA, p, q, node_sum, m2_elem);
        alpha = rho/inner_product_sparse(p, q, node_sum, m2_elem);

        axpy_solve_sparse(alpha, x_sparse, p, node_sum, m2_elem);
        axpy_solve_sparse(-alpha, r, q, node_sum, m2_elem);

    }

    printf("-----------------------X_SPARSE_CG-----------------------\n");
	for(i = 0; i < (node_sum - 1 + m2_elem); i++){
		printf("%lf\n", x_sparse[i]);
	}
}

void sparse_Bi_CG_analysis(){

}