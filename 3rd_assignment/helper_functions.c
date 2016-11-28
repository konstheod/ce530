#include "spice.h"

int if_SPD(int node_sum, int m2_elem){

    int i,j;
    gsl_matrix *l = gsl_matrix_alloc ((node_sum+m2_elem-1), (node_sum+m2_elem-1));
    for(i=0; i<(node_sum+m2_elem-1); i++){
        for(j=0; j<(node_sum+m2_elem-1); j++){
            gsl_matrix_set(l,i,j,gsl_matrix_get(mna,i,j));
        }
    }
    gsl_vector *eval = gsl_vector_alloc (node_sum+m2_elem-1);
    gsl_matrix *evec = gsl_matrix_alloc (node_sum+m2_elem-1, node_sum+m2_elem-1);

    gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc (node_sum+m2_elem-1);

    gsl_eigen_symmv (l, eval, evec, w);

    gsl_eigen_symmv_free (w);

    gsl_eigen_symmv_sort (eval, evec, 
                        GSL_EIGEN_SORT_ABS_ASC);

    int flag = 0;
    for (i = 0; i < (node_sum+m2_elem-1); i++){
        if(gsl_vector_get (eval, i) <= 0){ 
            flag = 1;
        }
    }

    if(flag == 1){
        printf("the matrix is not positive definite\n");
        gsl_vector_free (eval);
        gsl_matrix_free (evec);
        return -1;
    }
    else {
        gsl_vector_free (eval);
        gsl_matrix_free (evec);
        return 0;
    }



    gsl_vector_free (eval);
    gsl_matrix_free (evec);

  return 0;
}

void precondition_solver(gsl_vector *r, gsl_vector *z, gsl_vector *MNA_diag, int node_sum, int m2_elem){
    int i;

    for(i = 0; i<(node_sum+m2_elem-1); i++){
        if(gsl_vector_get(MNA_diag,i) <= EPS){
            gsl_vector_set(MNA_diag,i, 1);
        }
        gsl_vector_set(z,i, gsl_vector_get(r,i)/gsl_vector_get(MNA_diag,i));
    }
}

double inner_product(gsl_vector *r, gsl_vector *z, int node_sum, int m2_elem){
    double result = 0;
    int i;

    for(i = 0; i < (node_sum+m2_elem-1); i++ ){
        result += gsl_vector_get(r,i)*gsl_vector_get(z,i);
    }
    
    return result;
}

void mul_vector_matrix(gsl_vector *q, gsl_vector *p, int check, gsl_matrix *mnaT){
    if(check){
        gsl_blas_dgemv( CblasNoTrans, 1.0, mnaT, p, 0.0, q);
    }
    else{
        gsl_blas_dgemv( CblasNoTrans, 1.0, mna, p, 0.0, q);
    }
}

void axpy_solve(double alpha, gsl_vector *x, gsl_vector *y, int node_sum, int m2_elem){
    int i;
    gsl_vector *curr_y = gsl_vector_alloc(node_sum+m2_elem-1);
    for(i = 0; i<(node_sum+m2_elem-1); i++){
        gsl_vector_set(curr_y, i, gsl_vector_get(y,i));
    }

    gsl_blas_dscal(alpha, curr_y);
    gsl_vector_add(x,curr_y);

    gsl_vector_free(curr_y);
}

void get_diag_matrix(gsl_vector *MNA_diag, int node_sum, int m2_elem, int check, gsl_matrix *mnaT){
    int i,j;

    for(i = 0; i < (node_sum+m2_elem-1); i++){
        for(j = 0; j < (node_sum+m2_elem-1); j++){
            if(i==j){
                if(check){
                    gsl_vector_set(MNA_diag,i,gsl_matrix_get(mnaT,i,j));
                }
                else{
                    gsl_vector_set(MNA_diag,i,gsl_matrix_get(mna,i,j));
                }
            }
        }
    }
}

void matrix_transpose(gsl_matrix *dest, gsl_matrix *src, int node_sum, int m2_elem){
    int i;
    gsl_vector *row = gsl_vector_alloc(node_sum+m2_elem-1);

    for(i = 0; i<(node_sum+m2_elem-1); i++){
        gsl_matrix_get_row(row, src, i);
        gsl_matrix_set_col(dest, i, row);
    }   
    gsl_vector_free(row);
}

void get_b_sparse(double *b_sparse, int node_sum, int m2_elem){
    int i;

    for(i = 0; i < (node_sum - 1 + m2_elem); i++){
        b_sparse[i] = gsl_vector_get(b,i);
    }
}

void get_diag_matrix_sparse(cs_di *compressed_MNA, double *MNA_diag, int node_sum, int m2_elem){

    int j, k;

    for(j=0; j<(node_sum+m2_elem-1); j++){
        for(k = compressed_MNA->p[j]; k < compressed_MNA->p[j+1]; k++){
            if(j == compressed_MNA->i[k]){
                MNA_diag[j] = compressed_MNA->x[k];
                break;
            }
            else{
                MNA_diag[j] = 0;
            }
        }
    }
}

double norm_sparse(double *sparse, int node_sum, int m2_elem){
    int i;
    double norm_r;
    gsl_vector *r = gsl_vector_alloc(node_sum+m2_elem-1);
    
    for(i = 0; i < (node_sum-1+m2_elem); i++){
        gsl_vector_set(r,i,sparse[i]);
    }

    norm_r = gsl_blas_dnrm2(r);
    gsl_vector_free(r);

    return(norm_r);
}

void precondition_solver_sparse(double *r, double *z, double *MNA_diag, int node_sum, int m2_elem){
    int i;

    for(i = 0; i<(node_sum+m2_elem-1); i++){
        if(MNA_diag[i] <= EPS){
            MNA_diag[i] = 1;
        }
        z[i] = r[i]/MNA_diag[i];
    }
}

double inner_product_sparse(double *r, double *z, int node_sum, int m2_elem){
    double result = 0;
    int i;

    for(i = 0; i < (node_sum+m2_elem-1); i++ ){
        result += r[i]*z[i];
    }
    
    return result;
}

void mul_vector_matrix_sparse(cs_di *compressed_MNA, double *x, double *y, int node_sum, int m2_elem){
    int j,p;

    for(j = 0; j < (node_sum+m2_elem-1); j++ ){
        y[j] = 0.0;
        for(p = compressed_MNA->p[j]; p < compressed_MNA->p[j+1]; p++ ){
            y[j] = y[j] + compressed_MNA->x[p]*x[compressed_MNA->i[p]];
        }
    }

    
}

void axpy_solve_sparse(double alpha, double *x, double *y, int node_sum, int m2_elem){
    int i;
    double *curr_y = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    for(i = 0; i<(node_sum+m2_elem-1); i++){
        curr_y[i] = y[i];
    }

    for(i = 0; i<(node_sum+m2_elem-1); i++){
        y[i] = curr_y[i] + alpha*x[i] ;
    }

    free(curr_y);
}