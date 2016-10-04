#define SIZE_VALUE 20
#define SIZE_NAME 20
#define NODE_SIZE 40
#define PREC 15

struct element{
	char type;
	long int pos;
	long int neg;
	double value;
	long int D;
	long int G;
	long int S;
	long int B;
	long int C;
	long int E;
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
			printf("%d: type = %c, name = %s, pos = %s, neg = %s, model_name = %s, area = %.*lf \n", i, curr->type, curr->name, find_value(hash_head,curr->pos), find_value(hash_head,curr->neg), curr->model_name, PREC, curr->area);
		}
		else if(curr->type == 'M'){
			printf("%d: type = %c, name = %s, D = %s, G = %s, S = %s, B = %s, model_name = %s, L = %.*lf, W = %.*lf \n", i, curr->type, curr->name, find_value(hash_head,curr->D), find_value(hash_head,curr->G), find_value(hash_head,curr->S), find_value(hash_head,curr->B), curr->model_name, PREC, curr->L, PREC, curr->W);
		}
		else {
			printf("%d: type = %c, name = %s, C = %s, B = %s, E = %s, model_name = %s, area = %.*lf \n", i, curr->type, curr->name, find_value(hash_head,curr->C), find_value(hash_head,curr->B), find_value(hash_head,curr->E), curr->model_name, PREC, curr->area);
		}
		i++;
	}
	
}