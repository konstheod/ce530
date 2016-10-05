#include <stdlib.h> 
#define NODE_SIZE 40
struct node{
	char name[NODE_SIZE];
	unsigned long int key;
	struct node *next;
};

long int counter = 1;


long int find_node(struct node *head, char *name){
	struct node *curr;
	
	for(curr = head; curr!=NULL; curr = curr->next){
		if(strcmp(name,curr->name) == 0){
			return(curr->key);
		}
	}
	return(-1);
}

char* find_value(struct node *head, long int key){
	struct node *curr;
	
	for(curr = head; curr!=NULL; curr = curr->next){
		if(key == curr->key ){
			return(curr->name);
		}
	}
	return(NULL);
}

long int add_node(char *name, struct node **head){
	struct node *curr;
	int check;
	
	
	if((*head) != NULL){
		check = find_node(*head, name);
		//already exists
		if(check != -1){
			return (check);
		}
		curr = (struct node *) malloc (sizeof(struct node));
		if(curr == NULL){
			printf("Error at malloc (add_node)\n");
			return (-1);
		}
		curr->next = *head;
		*head = curr;
		(*head)->key = counter;
		strcpy((*head)->name,name);
		counter++;

	} 
	else {
		curr = (struct node *) malloc (sizeof(struct node));
		if(curr == NULL){
			printf("Error at malloc (add_node)\n");
			return (-1);
		}
		curr->next = NULL;
		*head = curr;
		(*head)->key = 0;
		strcpy((*head)->name,"0");
		
		if(strcmp(name,"0")){
			curr = (struct node *) malloc (sizeof(struct node));
			if(curr == NULL){
				printf("Error at malloc (add_node)\n");
				return (-1);
			}
			curr->next = *head;
			*head = curr;		
			(*head)->key = counter;
			strcpy((*head)->name,name);
			counter++;
		}
		
		
	}
	return((*head)->key);
}

int free_nodes(struct node **node_hash_head){
	struct node *hash_head, *next;
	hash_head = *node_hash_head;

	while(hash_head!=NULL){
		next = hash_head->next;
		free(hash_head);
		hash_head = next;
	}


	*node_hash_head = hash_head;
	return(1);
}