#include "spice.h"


gsl_matrix *mna;
gsl_vector *b;
gsl_vector *x_help;
gsl_vector *x;

int vol_counter = 0;

void MNA_init(int node_sum, int m2_elem){

	//calloc ston pinaka mna	
    mna = gsl_matrix_calloc ((node_sum+m2_elem-1), (node_sum+m2_elem-1));

    //calloc sto dianusma b
    b = gsl_vector_calloc (node_sum+m2_elem-1);

    x_help = gsl_vector_calloc (node_sum+m2_elem-1);

    x = gsl_vector_alloc ((node_sum+m2_elem-1));


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
        gsl_vector_set(x_help,(neg-1),cont->neg);
    	return(1);
    }
    if(neg == 0){
        gsl_matrix_set (mna, (pos-1), (pos-1), gsl_matrix_get(mna, (pos-1), (pos-1)) + value);
        gsl_vector_set(x_help,(pos-1),cont->pos);
    	return(1);
    }
    
    //anathesh timwn ston pinaka otan kanenas komvos den einai geiwsh
    gsl_matrix_set (mna, (neg-1), (neg-1), gsl_matrix_get(mna, (neg-1), (neg-1)) + value);
    gsl_matrix_set (mna, (pos-1), (pos-1), gsl_matrix_get(mna, (pos-1), (pos-1)) + value);

    gsl_matrix_set (mna, (pos-1), (neg-1), gsl_matrix_get(mna, (pos-1), (neg-1)) - value);
    gsl_matrix_set (mna, (neg-1), (pos-1), gsl_matrix_get(mna, (neg-1), (pos-1)) - value);

    gsl_vector_set(x_help,(neg-1),cont->neg);
    gsl_vector_set(x_help,(pos-1),cont->pos);
    
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
        gsl_vector_set(x_help,(neg-1),power->neg);
    	return 1;
    }
    if(neg == 0){
    	gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) - value);
        gsl_vector_set(x_help,(pos-1),power->pos);
    	return 1;
    }
   
    gsl_vector_set(b,(neg-1),gsl_vector_get(b,(neg-1)) + value);
    gsl_vector_set(b,(pos-1),gsl_vector_get(b,(pos-1)) - value);

    gsl_vector_set(x_help,(neg-1),power->neg);
    gsl_vector_set(x_help,(pos-1),power->pos);
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
        // printf("MNA_power_dc %lf\n", gsl_vector_get(b,(neg-1)));
   
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
    gsl_vector_set(x_help,(node_sum - 1 + vol_counter),0-atoi(vol->name));
    
    
    //prosthiki sto mna
    if(pos != 0){
        gsl_matrix_set (mna, (node_sum + vol_counter -1), (pos-1), gsl_matrix_get(mna, (node_sum + vol_counter -1), (pos-1)) + 1);
        gsl_matrix_set (mna, (pos-1), (node_sum + vol_counter -1), gsl_matrix_get(mna, (pos-1), (node_sum + vol_counter -1 )) + 1);
        gsl_vector_set(x_help,(pos-1),vol->pos);
    }
    if(neg != 0){
        gsl_matrix_set (mna, (node_sum + vol_counter -1), (neg-1), gsl_matrix_get(mna, (node_sum + vol_counter -1), (neg-1)) - 1);
        gsl_matrix_set (mna, (neg-1), (node_sum + vol_counter -1), gsl_matrix_get(mna, (neg-1), (node_sum + vol_counter -1)) - 1);
        gsl_vector_set(x_help,(neg-1),vol->neg);
    }
    vol->b_position = vol_counter;
    vol_counter++; 
    return 0;
}

int MNA_voltage_dc(struct element *vol,double value, int node_sum){

    //prosthiki sto dianusma b
    gsl_vector_set(b,(node_sum - 1 + vol->b_position),value);
    
    return 0;
}

void free_mna(){

    gsl_vector_free (x_help);
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
    printf("\n----b----\n");
    for(i=0; i<(node_sum+m2_elem-1); i++){
    
	   printf("%g \n",gsl_vector_get(b,i));
	
    }

    printf("\n----x_help----\n");
    for(i=0; i<(node_sum+m2_elem-1); i++){
    
       printf("%lu \n",find_index(gsl_vector_get(x_help,i)));
    
    }
    
    printf("\n");
}

void constructor(int node_sum, int m2_elem, struct element *head){
	MNA_init(node_sum, m2_elem);
	struct element *curr;
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
        if((norm_r/norm_b) <= itol || iter >= (node_sum+m2_elem-1)){
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
        if((norm_r/norm_b) <= itol || iter >= (node_sum+m2_elem-1)){
            break;
        }

        iter++;

        precondition_solver(r, z, MNA_diag, node_sum, m2_elem);
        precondition_solver(r1, z1, MNA_diagT, node_sum, m2_elem);

        rho = inner_product(r1, z, node_sum, m2_elem);
        if(fabs(rho) < EPS){
            printf("eeeeee %lf\n", fabs(rho));
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
            printf("AAA\n");
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