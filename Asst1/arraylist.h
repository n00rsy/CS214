//#include "arraylist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct token_t {
   char* str;
   int num;
} token;

typedef struct ArrayList_t{
   size_t total_size;
   size_t current_size;
   token** token_list;
} ArrayList;

void pstr(char* str){
   printf("'%s'\n",str);
}

void print_array(ArrayList * array){
   int i;
 // printf("%d\n", array->current_size);
   for(i = 0; i < array->current_size; i++){
         pstr(array->token_list[i]->str);
   }
}

// print first n elements of array
void printn(ArrayList * array,int n){
   int i;
   for(i = 0; i < n; i++){
         // pstr(array->token_list[i]->str);
         pstr(array->token_list[i]->str);
   }
}

void add(ArrayList * array,token *t){
   if(array->current_size >= array->total_size){
      token** old_list = array->token_list;
      int total_size_old = array->total_size;

      token** newList = malloc(sizeof(token)*array->total_size*2);
      int i;
      for(i=0; i< array->total_size;i++){
         newList[i] = malloc(sizeof(token));
         newList[i]->str = array->token_list[i]->str;
         newList[i]->num = array->token_list[i]->num;
      }
      // this fixes junk files in the bieginning

      for(i=0; i< array->total_size;i++){

	if(array->current_size <=i){
	  free(array->token_list[i]->str);
	}
	free(array->token_list[i]);
      }
      free(array->token_list);
      array->token_list = newList;
      array->total_size*=2;
   }

   //printf("YEEEE\n");
  // array->token_list[array->current_size] = malloc(sizeof(token));
   array->token_list[array->current_size] = t;
   array->current_size+=1;
}

ArrayList * init(size_t size){
   ArrayList * array = malloc(sizeof(ArrayList));
   array->total_size = size;
   array->current_size = 0;
   // create a list of tokens
   token** list_of_tokens = malloc(sizeof(token) * size);
   array->token_list = list_of_tokens;
   return array;
}

void end(ArrayList * array){
   int i;
   for(i = 0; i < array->total_size; i++){
     if(i< array->current_size){
      //may need to uncomment this
	// free(array->token_list[i]->str);
     }
     free(array->token_list[i]);
   }
   free(array->token_list);
   free(array);
}

char ** arrayListStrings(ArrayList * array){
   char ** strings=malloc(sizeof(char *)* array->current_size);
   int i;
   for(int i = 0;i<array->current_size;i++){
      strings[i] = malloc(sizeof(char)*strlen(array->token_list[i]->str));
      strcpy(strings[i],array->token_list[i]->str);
   }
   return strings;
}

int * arrayListInts(ArrayList * array){
   int * nums=malloc(sizeof(int)* array->current_size);
   int i;
   for(int i = 0;i<array->current_size;i++){

      printf("HLODKJLSDJK: %d\n", array->token_list[i]->num);
      nums[i] = array->token_list[i]->num;
   }
   return nums;
}