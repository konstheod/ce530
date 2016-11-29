#include "spice.h"

double *b_sparse;
double *x_sparse;

int CG_analysis(int node_sum, int m2_elem){
    int i;
    int iter = 0;
    double norm_r, norm_b;
    double rho = 0;
    double beta = 0;
    double rho1 = 0;
    double alpha = 0;
    int check = 1;

    check = if_SPD(node_sum,m2_elem);
    if(check == -1){
        return -1;
    }

    gsl_vector *r = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *z = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *p = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *q = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *MNA_diag = gsl_vector_alloc(node_sum+m2_elem-1);
    get_diag_matrix(MNA_diag, node_sum, m2_elem, 0, NULL);
    for(i = 0; i<(node_sum+m2_elem-1); i++){
        gsl_vector_set(x,i,0);
        gsl_vector_set(q,i,0);
        gsl_vector_set(r,i,gsl_vector_get(b,i));
    }

    norm_b = gsl_blas_dnrm2(b);
    if(fabs(norm_b) <= EPS){
        norm_b = 1;
    }
    while(1){

        norm_r = gsl_blas_dnrm2(r);
        if((norm_r/norm_b) <= itol || iter >= (node_sum+m2_elem)*2){
            break;
        }

        iter++;

        precondition_solver(r, z, MNA_diag, node_sum, m2_elem);


        rho = inner_product(r, z, node_sum, m2_elem);

        if(iter == 1) {
            gsl_vector_memcpy(p,z);
        }
        else {
            beta = rho/rho1;
            gsl_blas_dscal(beta, p);
            gsl_vector_add(p,z);
        }

        rho1 = rho;
        mul_vector_matrix(q, p, 0, NULL);
        
        alpha = rho/inner_product(p, q, node_sum, m2_elem);

        axpy_solve(alpha, x, p, node_sum, m2_elem);
        axpy_solve(-alpha, r, q, node_sum, m2_elem);
        

    }

    gsl_vector_free (r);
    gsl_vector_free (z);
    gsl_vector_free (p);
    gsl_vector_free (q);
    gsl_vector_free (MNA_diag);

    return 0;
}

int Bi_CG_analysis(int node_sum, int m2_elem){
    int i;
    int iter = 0;
    double norm_r, norm_b;
    double rho = 0;
    double beta = 0;
    double rho1 = 0;
    double alpha = 0;
    double omega = 0;
    

    gsl_vector *r = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *r1 = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *z = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *z1 = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *p = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *p1 = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *q = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *q1 = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_vector *MNA_diag = gsl_vector_alloc(node_sum+m2_elem-1);
    gsl_matrix *mnaT = gsl_matrix_alloc((node_sum+m2_elem-1),(node_sum+m2_elem-1));
    gsl_vector *MNA_diagT = gsl_vector_alloc(node_sum+m2_elem-1);
    get_diag_matrix(MNA_diag, node_sum, m2_elem, 0, NULL);
    matrix_transpose(mnaT, mna, node_sum, m2_elem);
    get_diag_matrix(MNA_diagT, node_sum, m2_elem, 1, mnaT);

    for(i = 0; i<(node_sum+m2_elem-1); i++){
        gsl_vector_set(x,i,0);
        gsl_vector_set(q,i,0);
        gsl_vector_set(q1,i,0);
        gsl_vector_set(p,i,0);
        gsl_vector_set(p1,i,0);
        gsl_vector_set(z,i,0);
        gsl_vector_set(z1,i,0);
        gsl_vector_set(r,i,gsl_vector_get(b,i));
        gsl_vector_set(r1,i,gsl_vector_get(b,i));
    }

    norm_b = gsl_blas_dnrm2(b);
    if(fabs(norm_b) < EPS){
        norm_b = 1;
    }
    while(1){

        norm_r = gsl_blas_dnrm2(r);
        if((norm_r/norm_b) <= itol || iter >= (node_sum+m2_elem)*2){
            break;
        }

        iter++;

        precondition_solver(r, z, MNA_diag, node_sum, m2_elem);
        precondition_solver(r1, z1, MNA_diagT, node_sum, m2_elem);

        rho = inner_product(r1, z, node_sum, m2_elem);
        if(fabs(rho) < EPS){
            return -1;
        }

        if(iter == 1) {
            gsl_vector_memcpy(p,z);
            gsl_vector_memcpy(p1,z1);
        }
        else {
            beta = rho/rho1;
            gsl_blas_dscal(beta, p);
            gsl_vector_add(p,z);
            gsl_blas_dscal(beta, p1);
            gsl_vector_add(p1,z1);
        }

        rho1 = rho;
        mul_vector_matrix(q, p, 0, NULL);
        mul_vector_matrix(q1, p1,1,mnaT);
        omega = inner_product(p1, q, node_sum, m2_elem);
        if(fabs(omega) < EPS){
            return -1;
        }
        alpha = rho/omega;
        axpy_solve(alpha, x, p, node_sum, m2_elem);
        axpy_solve(-alpha, r, q, node_sum, m2_elem);
        axpy_solve(-alpha, r1, q1, node_sum, m2_elem);

    }

    gsl_vector_free (r);
    gsl_vector_free (z);
    gsl_vector_free (p);
    gsl_vector_free (q);
    gsl_vector_free (MNA_diag);
    gsl_vector_free (r1);
    gsl_vector_free (z1);
    gsl_vector_free (p1);
    gsl_vector_free (q1);
    gsl_vector_free (MNA_diagT);
    gsl_matrix_free (mnaT);
    return 0;
}

void sparse_CG_analysis(cs_di *compressed_MNA, int node_sum, int m2_elem){
    int i;
    int iter = 0;
    double norm_r, norm_b;
    double rho = 0;
    double beta = 0;
    double rho1 = 0;
    double alpha = 0;


    double *r = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *z = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *p = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *q = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *MNA_diag = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    get_diag_matrix_sparse(compressed_MNA, MNA_diag, node_sum, m2_elem);

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

        axpy_solve_sparse(alpha, p, x_sparse, node_sum, m2_elem);
        axpy_solve_sparse(-alpha, q, r, node_sum, m2_elem);


    }

    sparse_set_x(node_sum, m2_elem);

    free(r);
    free(p);
    free(q);
    free(z);
    free(MNA_diag);
}


void sparse_Bi_CG_analysis(cs_di *compressed_MNA, int node_sum, int m2_elem){
    int i;
    int iter = 0;
    double norm_r, norm_b;
    double rho = 0;
    double beta = 0;
    double rho1 = 0;
    double alpha = 0;


    double *r = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *r1 = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *z = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *z1 = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *p = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *p1 = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *q = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *q1 = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    double *MNA_diag = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    get_diag_matrix_sparse(compressed_MNA, MNA_diag, node_sum, m2_elem);

    cs_di *compressed_MNA_T = cs_di_transpose (compressed_MNA, (node_sum+m2_elem-1)*(node_sum+m2_elem-1));
    double *MNA_diag_T = (double *) malloc((node_sum+m2_elem-1)*sizeof(double));
    get_diag_matrix_sparse(compressed_MNA_T, MNA_diag_T, node_sum, m2_elem);

    for(i = 0; i<(node_sum+m2_elem-1); i++){
        x_sparse[i] = 0;
        q[i] = 0;
        q1[i] = 0;
        r[i] = b_sparse[i];
        r1[i] = b_sparse[i];
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
        precondition_solver_sparse(r1, z1, MNA_diag, node_sum, m2_elem);

        rho = inner_product_sparse(r, z, node_sum, m2_elem);
        if(fabs(rho) < EPS){
            return;
        }

        if(iter == 1) {
            for(i = 0; i<(node_sum+m2_elem-1); i++){
                p[i] = z[i];
                p1[i] = z1[i];
            }
        }
        else {
            beta = rho/rho1;
            
            for(i = 0; i<(node_sum+m2_elem-1); i++){
                p[i] = z[i] + beta*p[i];
                p1[i] = z1[i] + beta*p1[i];
            }
        }

        rho1 = rho;
        mul_vector_matrix_sparse(compressed_MNA, p, q, node_sum, m2_elem);
        mul_vector_matrix_sparse(compressed_MNA_T, p1, q1, node_sum, m2_elem);

        alpha = rho/inner_product_sparse(p1, q, node_sum, m2_elem);

        axpy_solve_sparse(alpha, p, x_sparse, node_sum, m2_elem);
        axpy_solve_sparse(-alpha, q, r, node_sum, m2_elem);
        axpy_solve_sparse(-alpha, q1, r1, node_sum, m2_elem);


    }

    sparse_set_x(node_sum, m2_elem);

    free(r);
    free(r1);
    free(p);
    free(p1);
    free(q);
    free(q1);
    free(z);
    free(z1);
    free(MNA_diag);
    free(MNA_diag_T);
    cs_di_spfree(compressed_MNA_T);
}