#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#define INT_MAX 2147483647

void pnum(int num){
   printf("%d\n",num);
}

void pstr(char* str){
   printf("%s\n",str);
}

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

void add(token *t);

void init(size_t size){
   array = malloc(sizeof(ArrayList));
   array->total_size = size;
   array->current_cap = 0;
   array->is_num = 1;
   // create a list of tokens
   token** list_of_tokens = malloc(sizeof(token) * size);
   int i; 
   for(i = 0; i < array->total_size; i++){
      list_of_tokens[i] = malloc(sizeof(token));
      list_of_tokens[i]->str = malloc(sizeof(char) * 10);
      list_of_tokens[i]->str = 3;
   }
  array->token_list = list_of_tokens;
}

void print_array(){
   int i;
   for(i = 0; i < array->total_size; i++){
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

   token *test = malloc(sizeof(token));
   test->str = "3";
   pnum(checkIfInt(test));

}


