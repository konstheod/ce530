#include "spice.h"

long int counter = 1;

struct node * hash_table[HASH_TABLE_SIZE];

unsigned int hash(char *name){
   unsigned int hash_code = 5381;
   int i = 0;
   for(i = 0; name[i] != '\0'; i++){
	hash_code = (hash_code << 5) + hash_code + name[i];
   }
   return (hash_code%HASH_TABLE_SIZE);
    
}

long int find_node(char *name){
	
	int hashIndex = hash(name);  
	
   while(hash_table[hashIndex] != NULL){
	
      if(strcmp(hash_table[hashIndex]->name, name)==0)
         return hash_table[hashIndex]->key; 
			
      ++hashIndex;
		
      hashIndex %= HASH_TABLE_SIZE;
   }    
	return(-1);
}

char* find_value( long int key){
	if(key == 0){
		return ("0");
	}
	return(hash_table[key%HASH_TABLE_SIZE]->name);
}
unsigned long int find_index( long int key){
	if(key == 0){
		return (0);
	}
	return(hash_table[key%HASH_TABLE_SIZE]->index);
}

long int add_node(char *name){
	struct node *curr;
	unsigned int hashIndex; 
	int check;

	if(!strcmp(name, "0")) {
	    return 0;
	}
	check = find_node(name);
	if(check!= (-1)){
		return(check);
	}
	hashIndex = hash(name);
	curr = (struct node *) malloc (sizeof(struct node));
	strcpy(curr->name,name);
	curr->key = hashIndex;
	curr->index = counter;

	while(hash_table[hashIndex] != NULL){
      ++hashIndex;
		
      hashIndex %= HASH_TABLE_SIZE;
   }

   	counter++;
   	hash_table[hashIndex] = curr;      
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