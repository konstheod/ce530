#include "spice.h"

long int counter = 1;

struct node * hash_table[HASH_TABLE_SIZE];

unsigned long int hash(char *name){
   unsigned long int hash_code = 5381;
   int i = 0;
   for(i = 0; name[i] != '\0'; i++){
	hash_code = (hash_code << 5) + hash_code + name[i];
   }
   return (hash_code);
    
}

unsigned long int find_node(char *name){
	
	int hashIndex = hash(name)%HASH_TABLE_SIZE;  
	struct node *curr, *head;

	head = hash_table[hashIndex];
	if(head==NULL){
		return(-1);
	}

	for(curr = head; curr != NULL; curr = curr->next){
		if(strcmp(name,curr->name) == 0){
			return curr->key;
		}
	}
	   
	return(-1);
}

char* find_value( unsigned long int key){
	int hashIndex = key%HASH_TABLE_SIZE;
	struct node *curr, *head;
	// printf("find value key = %ld\n",key );
	head = hash_table[hashIndex];

	if(key == 0){
		return ("0");
	}

	for(curr = head; curr != NULL; curr = curr->next){
		if(key == curr->key){
			return curr->name;
		}
	}

	return(NULL);
}
unsigned long int find_index( unsigned long int key){
	struct node *curr, *head;

	if(key == 0){
		return (0);
	}
	int hashIndex = key%HASH_TABLE_SIZE;

	head = hash_table[hashIndex];

	for(curr = head; curr != NULL; curr = curr->next){
		if(key == curr->key){
			return curr->index;
		}
	}

	return(-1);
}

unsigned long int add_node(char *name){
	struct node *curr;
	unsigned int hashIndex;
	unsigned long int key; 
	unsigned long int check;

	if(!strcmp(name, "0")) {
	    return 0;
	}
	check = find_node(name);
	if(check!= (-1)){
		return(check);
	}

	key = hash(name);
	hashIndex = key%HASH_TABLE_SIZE;

	curr = (struct node *) malloc (sizeof(struct node));
	strcpy(curr->name,name);
	curr->key = key;
	curr->index = counter;
	curr->next = hash_table[hashIndex];
	hash_table[hashIndex] = curr;

   	counter++;    
	return(curr->key);
}

int free_nodes(struct node * hash_table[]){
	struct node *hash_head, *next;
	
	int i; 
	
	for(i = 0; i < HASH_TABLE_SIZE; i++){
	    hash_head = hash_table[i];
	    while(hash_head != NULL){
			next = hash_head->next;
			free(hash_head);
			hash_head = next;
		}
		hash_table[i] = hash_head;
	}
		
	return(1);
}

int nodes(){
	return(counter);
}