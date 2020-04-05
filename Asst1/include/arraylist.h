#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef ARRAYLIST_H
#define ARRAYLIST_H

typedef struct token_t {
   char* str;
   int num;
} token;

typedef struct ArrayList_t {
   size_t total_size;
   size_t current_size;
   token** token_list;
} ArrayList;

void pstr(char* str);
void pnum(int num);
void print_array(ArrayList * array);
void printn(ArrayList * array,int n);
void add(ArrayList * array,token *t);

ArrayList * init(size_t size);

void end(ArrayList * array);
char ** arrayListStrings(ArrayList * array);
int * arrayListInts(ArrayList * array);

#endif
