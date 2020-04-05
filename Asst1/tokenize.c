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
void strToArrayList(char * s, ArrayList *array);

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
void buildCodebook(ArrayList *array, char *path){
/*
1. traverse files and build array
buildHuffmanFromArrayList(array);
*/

   DIR* d;
   d = opendir(path);
   struct dirent *dir;
   while((dir = readdir(d)) != NULL){
    if(dir->d_type == DT_DIR){
     if(is_rec(dir->d_name)){//check if recursive here
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
   strToArrayList(buffer, array);
   close(f);
   }
  }
   closedir(d);
} 


// tokenize file
void strToArrayList(char * s, ArrayList *array){
   int p1 = 0;
   int p2 = 0;
   while(s[p2] != '\0'){
      //scan token
      while(s[p2] != '\0' && s[p2] != ' ' && s[p2]!= '\t' && s[p2]!= '\n' ){
         p2++;
      }
      // add spaces, tabs, and newlines as tokens
      token *t = malloc(sizeof(token));
      switch(s[p2]){
        case ' ':
               t->str = ' ';
               add(array,t);
               break; 
        case '\t':
               t->str = '\t' 
               add(array,t);
               break; 
        case '\n':
               t->str = '\n' 
               add(array,t);
               break; 
      } 

      //extract token and put in data structure
      char *str = substring( s, p1 + 1, p2 - p1);

      // printf("%s\n", str);
      token *t = malloc(sizeof(token));
      t->str = str;
      add(array,t);
      while(s[p2] != ',' && s[p2] != '\0'){
         p2++;
      }
      //move past comma, whitespace then reset
      //printf("p2 is at: %c\n", s[p2]);
      while(s[p2]== ' ' || s[p2]== ',' || s[p2]== '\t' || s[p2]== '\n'){
         p2++;
      }
     // printf("p2 is at: after loop '%c'\n", s[p2]);
      p1=p2;
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
