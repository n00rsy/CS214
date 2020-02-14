#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#define INT_MAX 2147483647
#define pnum(num) printf("%d\n",num)
#define pstr(str) printf("%s\n",str)

typedef union token_t {
   char* str;
   size_t len_str;
} token; 

typedef struct ArrayList_t{
   // 0 if string, anything else if num
   int is_num;
   // total size of the Array allocated
   size_t total_size;
   // number of tokens actually used in the array
   size_t current_cap;
   // list of tokens
   token** token_list;
} ArrayList;
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

void add(ArrayList *a, token t);


ArrayList* init(size_t size){
   ArrayList* a = malloc(sizeof(ArrayList)); 
   a->total_size = size;
   a->current_cap = 0;
   a->is_num = 0;
   // create a list of tokens
   token** list_of_tokens = malloc(sizeof(token) * size);
   for(int i = 0; i < a->total_size; i++){
      list_of_tokens[i] = malloc(sizeof(token));
      list_of_tokens[i]->str = malloc(sizeof(char) * 10);
      list_of_tokens[i]->str = 3;
   }
  a->token_list = list_of_tokens;
}

void print_array(ArrayList *list){
   int i;
   for(i = 0; i < list->total_size; i++){
      if(list->is_num){
         pnum(list->token_list[i]->str);
      } else {
         pstr(list->token_list[i]->str);
      }
   }
}

int main (int argc, char * argv[]){
    ArrayList* list = init(32);
    print_array(list);

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

   char *c = *(char *) calloc(INT_MAX, sizeof(char));
   while(read(file,c,INT_MAX)>0){
      printf("%s\n", c);
      printf("AAAAAA\n");
   }
}




char *a = "hello";
