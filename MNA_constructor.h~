extern double **mna;
extern double *b;

int vol_counter = 0;

void MNA_init(int node_sum, int m2_elem){
	int i;

	//calloc ston pinaka mna
	mna = (double**) calloc((node_sum+m2_elem-1),sizeof(double));
	if(mna == NULL){
		printf("Error to malloc!\n");
		exit(-1);
	}
	for(i=0; i<(node_sum+m2_elem-1); i++){
		mna[i] = (double*) calloc((node_sum+m2_elem-1),sizeof(double));
		if(mna[i] == NULL){
			printf("Error to malloc!\n");
			exit(-1);
		}
	}

	//calloc sto dianusma b
	b = (double*) calloc((node_sum+m2_elem-1),sizeof(double));
	if(b == NULL){
		printf("Error to malloc!\n");
		exit(-1);
	}

}

int MNA_conductance(struct element *cont){
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
	mna[neg-1][neg-1] += value;
	return(1);
    }
    if(neg == 0){
	mna[pos-1][pos-1] += value;
	return(1);
    }
    
    //anathesh timwn ston pinaka otan kanenas komvos den einai geiwsh
    mna[pos-1][pos-1] += value;
    mna[neg-1][neg-1] += value;

    mna[neg-1][pos-1] -= value;
    mna[pos-1][neg-1] -= value;
    
    return(0);
}

int MNA_power(struct element *power){
   unsigned long int pos, neg;
   double value;
    
    //pairnoume thn timh ths adistashs kai th metatrepoume se agwgimothta gia na thn prosthesoume ston pinaka
    value = power->value;
    
    //pairnoume tous komvous tou stoixeiou mesa apo to hash_table wste na topothethsoume to value sth swsth thesh ston pinaka
    pos = find_index(power->pos);
    neg = find_index(power->neg); 
    
    if(pos == 0){
	b[neg-1] += value;
	return 1;
    }
    if(neg == 0){
	b[pos-1] -= value;
	return 1;
    }
   
    b[neg-1] += value;
    b[pos-1] -= value;
    return 0;

}

int MNA_voltage(struct element *vol, int node_sum){
   unsigned long int pos, neg;
   double value;
    
   
    value = vol->value;
    
    //prosthiki sto dianusma b
    pos = find_index(vol->pos);
    neg = find_index(vol->neg); 
    
    if(vol->type == 'L'){
	value = 0;
    }
    
    b[node_sum - 1 + vol_counter] = value;
    
    vol_counter++; 
    
    //prosthiki sto mna
    if(pos == 0){
	mna[node_sum-1+neg-1][0] = -1;
	mna[0][node_sum-1+neg-1] = -1;
	return 1;
    }
    if(neg == 0){
	mna[node_sum-1+pos-1][0] = 1;
	mna[0][node_sum-1+pos-1] = 1;
	return 1;
    }
    mna[node_sum-1+neg-1][0] = -1;
    mna[0][node_sum-1+neg-1] = -1;
    mna[node_sum-1+pos-1][0] = 1;
    mna[0][node_sum-1+pos-1] = 1;
    
    return 0;
}

void free_mna(int node_sum, int m2_elem){
    int i;
    
    
    
    for(i=0; i<(node_sum+m2_elem-1); i++){
    
	free(mna[i]);
	
    }
    free(mna);
    free(b);
}
void print_MNA(int node_sum, int m2_elem){
    int i,j;
    
    printf("----MNA----\n");
    for(i=0; i<(node_sum+m2_elem-1); i++){
	for(j=0; j<(node_sum+m2_elem-1); j++){
	    printf("%lf  ",mna[i][j]);
	}
	printf("\n");
    }
    printf("----b----\n");
    for(i=0; i<(node_sum+m2_elem-1); i++){
    
	printf("%lf  ",b[i]);
	
    }
   
    
    printf("\n");
}
void constructor(int node_sum, int m2_elem, struct element *head){
	MNA_init(node_sum, m2_elem);
	struct element *curr;
	
	for(curr = head; curr != NULL; curr = curr->next){
	    if(curr->type == 'R'){
		MNA_conductance(curr);
// 		print_MNA(node_sum,m2_elem);
	    }
	    else if(curr->type == 'I') {
		MNA_power(curr);
	    }
	    else if(curr->type == 'V' || curr->type == 'L') {
		MNA_voltage(curr, node_sum);
	    }
	}
	
	
}
