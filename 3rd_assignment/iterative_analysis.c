#include "spice.h"

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
    for(i=0; i<(node_sum+m2_elem-1); i++){
    
       printf("%g \n",gsl_vector_get(MNA_diag,i));
    
    }
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