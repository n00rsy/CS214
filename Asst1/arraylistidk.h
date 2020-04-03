#include <stdlib.h>
#include <stdio.h>

#pragma once

typedef union token_t {
   char* str;
   int num;
} token;

typedef struct ArrayList_t{
   int is_num;
   size_t total_size;
   size_t current_size;
   token** token_list;
} ArrayList;

ArrayList *array;

void print_array();
void printn(int n);
void add(token *t);
void init(size_t size);
void end();
