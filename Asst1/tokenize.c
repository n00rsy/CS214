#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "./include/arraylist.h"
#include "./include/huffman.h"

const int INT_MAX = 2147483647;
const char* HOME = ".";
const char* PARENT = "..";

int is_rec(char *path){
   if(strcmp(path,HOME) == 0 || strcmp(path,PARENT) == 0){ 
      return 0;
   }
   return 1;
}

void buildCodebook(ArrayList *array, char *path);
char *substring(char *string, int position, int length);
void strToArrayListWithCount(char * s, ArrayList *array);

void buildCodebookDriver(){
  ArrayList *array = init(10); 
  buildCodebook(array, ".");
  print_array(array);
  buildHuffmanFromArrayList(array);
}

char *my_strcpy(char *destination, char *source)
{
  char *start = destination;

  while(*source != '\0')
  {
    *destination = *source;
    destination++;
    source++;
  }

    *destination = '\0'; // add '\0' at the end
    return start;
}

// starts the recursion
void buildCodebook(ArrayList *array, char *path, int recursive){
/*
1. traverse files and build array
buildHuffmanFromArrayList(array);
*/

   DIR* d;
   d = opendir(path);
   struct dirent *dir;
   while((dir = readdir(d)) != NULL){
    if(dir->d_type == DT_DIR){
     if(is_rec(dir->d_name)&&recursive ==1){//check if recursive here
      // printf("directory: %s\n", dir->d_name);
      char  fullPath[100];
      my_strcpy(fullPath, path);
      strcat(fullPath,"/");
      strcat(fullPath, dir->d_name);	   
      buildCodebook(array,fullPath);
    }
  } else {
    // its a file so we can start reading it and adding tokens
   // printf("file: %s:\n", dir->d_name);
   int f = open(dir->d_name, O_RDONLY);
   if(f == -1){
      printf("File does not exist\n");
      return;
   }

   // get file data so we can see how long it is
   struct stat st; 
   stat(dir->d_name, &st);
   size_t file_len = st.st_size;
   printf("printing %s\n", dir->d_name);
   char buffer[file_len];
   while(read(f,buffer,file_len) > 0);
   strToArrayListWithCount(buffer, array);
   close(f);
   }
  }
   closedir(d);
} 


// tokenize file
void strToArrayListWithCount(char * s, ArrayList *array){
  
 int p1 = 0;
 int p2 = 0;
   while(s[p2] != '\0'){

    //if(s[p2]== ' ' && s[p2]== ',' && s[p2]== '\t' && s[p2]!= '\n')
    char *t;
    if(s[p2]== ' '){
      t = "\\";
    }
    else if(s[p2]== '\t'){
      t = "\\t";
    }
    else if(s[p2]== '\n'){
      t = "\\n";
    }

    else{
    while(s[p2] != '\0'&&  s[p2]!= ' ' && s[p2]!= ',' && s[p2]!= '\t' && s[p2]!= '\n' ){
         p2++;
      }
      t = substring( s, p1 + 1, p2 - p1);
    }
    p2++;
    p1=p2;
//for each token{
    int add = 0;
for(i = 0;i<array->current_size;i++){
 if(strcmp(t,array->token_list[i]->str)==0){
        int new = array->token_list[i]->num+1;
        array->token_list[i]->num = new;
        add = 1;
        char str[12];
        sprintf(str, "%d", array->token_list[i]->num);
   }
 }
 if(add ==0){

 token *to = malloc(sizeof(token));
 to->str = str;
 to->num = 1;
 add(array,to);
 }
}
}

char *substring(char *string, int position, int length){
   char *pointer;
   int c;

   pointer = malloc(length+1);

   if (pointer == NULL)
   {
      printf("Unable to allocate memory.\n");
      exit(1);
   }

   for (c = 0 ; c < length ; c++)
   {
      *(pointer+c) = *(string+position-1);      
      string++;  
   }

   *(pointer+c) = '\0';

   return pointer;
}

int main(){
 buildCodebookDriver();
}
