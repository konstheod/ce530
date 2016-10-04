#define SIZE_VALUE 20
#define SIZE_NAME 20
#define NODE_SIZE 40
#define PREC 15

struct element{
	char type;
	long int pos;
	long int neg;
	double value;
	int D;
	int G;
	int S;
	int B;
	int C;
	int E;
	double L;
	double W;
	char model_name[SIZE_NAME];
	double area;
	char name[SIZE_NAME];
	struct element *next;
};


void printList(struct element *head, struct node *hash_head){
	struct element *curr;
	int i = 1;
	
	for(curr = head; curr != NULL; curr = curr->next){
		if(curr->type == 'R' || curr->type == 'C' || curr->type == 'L' || curr->type == 'V' || curr->type == 'I'){
			printf("%d: type = %c, name = %s, pos = %s, neg = %s, value = %.*lf \n",i, curr->type, curr->name, find_value(hash_head,curr->pos), find_value(hash_head,curr->neg), PREC,curr->value);
		}
		else if(curr->type == 'D'){
			printf("%d: type = %c, name = %s, pos = %ld, neg = %ld, model_name = %s, area = %.*lf \n", i, curr->type, curr->name, curr->pos, curr->neg, curr->model_name, PREC, curr->area);
		}
		else if(curr->type == 'M'){
			printf("%d: type = %c, name = %s, D = %d, G = %d, S = %d, B = %d, model_name = %s, L = %.*lf, W = %.*lf \n", i, curr->type, curr->name, curr->D, curr->G, curr->S, curr->B, curr->model_name, PREC, curr->L, PREC, curr->W);
		}
		else {
			printf("%d: type = %c, name = %s, C = %d, B = %d, E = %d, model_name = %s, area = %.*lf \n", i, curr->type, curr->name, curr->C, curr->B, curr->E, curr->model_name, PREC, curr->area);
		}
		i++;
	}
	
}