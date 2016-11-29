#include "spice.h"

double *b_sparse;
double *x_sparse;

int LU_analysis(int node_sum,int m2_elem){
    int s, i, j;

    gsl_matrix *l = gsl_matrix_alloc ((node_sum+m2_elem-1), (node_sum+m2_elem-1));
    gsl_permutation * p = gsl_permutation_alloc ((node_sum+m2_elem-1));

    for(i=0; i<(node_sum+m2_elem-1); i++){
        for(j=0; j<(node_sum+m2_elem-1); j++){
            gsl_matrix_set(l,i,j,gsl_matrix_get(mna,i,j));
        }
    }

    gsl_linalg_LU_decomp (l, p, &s);
    
    gsl_linalg_LU_solve (l, p, b, x);
    gsl_permutation_free (p);
    gsl_matrix_free(l);

    return (0);
}

int Cholesky_analysis(int node_sum,int m2_elem){

    int check = 1, i , j;

    check = if_SPD(node_sum,m2_elem);
    if(check == -1){
        return -1;
    }

    gsl_matrix *l = gsl_matrix_alloc ((node_sum+m2_elem-1), (node_sum+m2_elem-1));

    for(i=0; i<(node_sum+m2_elem-1); i++){
        for(j=0; j<(node_sum+m2_elem-1); j++){
            gsl_matrix_set(l,i,j,gsl_matrix_get(mna,i,j));
        }
    }
    

    check = gsl_linalg_cholesky_decomp(l);
    if(check != GSL_SUCCESS){   
        printf("%d\n", check );
        return -2;
    }
    
    

    gsl_linalg_cholesky_solve (l, b, x);
    // printf ("x = \n");
    // gsl_vector_fprintf (stdout, x, "%g");
    gsl_matrix_free(l);

    return (0);
}

void sparse_LU_analysis(cs_di *compressed_MNA, int node_sum, int m2_elem){
    cs_dis *S;
    cs_din *N;

    S = NULL;
    N = NULL;

    S = cs_di_sqr(2, compressed_MNA, 0);
    N = cs_di_lu(compressed_MNA, S, 1);



    cs_di_ipvec(N->pinv, b_sparse, x_sparse, node_sum - 1 + m2_elem);
    cs_di_lsolve(N->L, x_sparse);
    cs_di_usolve(N->U, x_sparse);
    cs_di_ipvec(S->q, x_sparse, b_sparse, node_sum - 1 + m2_elem);

    sparse_set_x(node_sum, m2_elem);

    cs_di_sfree(S);
    cs_di_nfree(N);
}

void sparse_Cholesky_analysis(cs_di *compressed_MNA, int node_sum, int m2_elem){
    cs_dis *S;
    cs_din *N;

    S = NULL;
    N = NULL;
    
    S = cs_di_schol(1, compressed_MNA);
    N = cs_di_chol(compressed_MNA, S);


    cs_di_ipvec(S->pinv, b_sparse, x_sparse, node_sum - 1 + m2_elem);
    cs_di_lsolve(N->L, x_sparse);
    cs_di_ltsolve(N->L, x_sparse);
    cs_di_pvec(S->pinv, x_sparse, b_sparse, node_sum - 1 + m2_elem);

    sparse_set_x(node_sum, m2_elem);

    cs_di_sfree(S);
    cs_di_nfree(N);
}
