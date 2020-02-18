#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#define INT_MAX 1000000000
//2147483647
void pnum(int num){
   printf("%d\n",num);
}

void pstr(char* str){
   printf("%s\n",str);
}

typedef union token_t {
   char* str;
   int num;
} token;

typedef struct ArrayList_t{
   // 0 if string, anything else if num
   int is_num;
   // total size of the Array allocated
   size_t total_size;
   // number of tokens actually used in the array
   size_t current_size;
   // list of tokens
   token** token_list;
} ArrayList;

ArrayList *array;
/*
   make a method to declare a new arraylist
   make another method for add ( handle overlfow)

   1) open file put into char array
   2) determine if num or string parse char array into ArrayList
   3) sort


   1) 2 pointers until comma
   2) count len of token
   3) allocate memory for token
   4) add to data structure


*/

size_t strLength(char * s){

	int c = 0;
	while(s[c] != '\0'){
		c++;
	}
	return c;
}

void add(token *t){

	if(array->current_size>=array->total_size){
		//make arrray bigger		
		token** newList = malloc(sizeof(token)*array->total_size*2);
		int i;
		for(i=0; i< array->total_size;i++){
			newList[i] = malloc(sizeof(token));
			size_t strLen = strLength(array->token_list[i]->str);
			newList[i]-> str = malloc(sizeof(char)*strLen+1);
			newList[i]->str[strLen] = '\0';
			char j;
			for(j=0;array->token_list[i]->str[j] != '\0';++j){
				newList[i]->str[j] = array->token_list[i]->str[j];
			}
		}
		array->total_size*=2;
	}
	
	array->token_list[array->current_size] = malloc(sizeof(token));
	array->token_list[array->current_size] = t;
	array->current_size+=1;

>>>>>>> 96bf512d9a69eed8ae0a7f2f0b4f8dc4067eb623
}

void init(size_t size){

   array = malloc(sizeof(ArrayList));
   array->total_size = size;
   array->current_size = 0;
   array->is_num = 0;

   // create a list of tokens
   token** list_of_tokens = malloc(sizeof(token) * size);
   int i; 
   for(i = 0; i < array->total_size; i++){
      list_of_tokens[i] = malloc(sizeof(token));
   }
  array->token_list = list_of_tokens;
}

void print_array(){
   int i;
   for(i = 0; i < array->current_size; i++){
      if(array->is_num){
         pnum(array->token_list[i]->str);
      } else {
        pstr(array->token_list[i]->str);
      }
   }
}

int checkIfInt(token * t){
  if(t->str[0]>=48 && t->str[0]<=57){
    return 1;
  }
  return 0;

}



int main (int argc, char * argv[]){
   init(10);

   int i;
   for(i=0;i<500;i++){
	token *test = malloc(sizeof(token));
   	if(i%2==0){
	test->str = "NOOR";
	}
	else if(i%3==0){
	test->str = "LIKES";
	}
	else{
	test->str = "RIDHWAAN";
	}	
	add(test);
}   
printf("FINISHED ADDING\n");
   print_array(array);

   if(argc!=3){
      //throw error, incorrect arguements
      printf("Incorrect arguements\n");
      return 1;
   }
   int file = open(argv[2], O_RDONLY);
   if(file==-1){
      //throw error, file doesnt exist
      printf("File does not exist\n");
      return 1;
   }

   char *c = (char *) calloc(INT_MAX, sizeof(char));
   while(read(file,c,INT_MAX)>0){
       // printf("%s\n", c);
       // printf("%s\n", "AAAAA");
   }

   //token *test = malloc(sizeof(token));
   //test->str = "3";
   //pnum(checkIfInt(test));

}


