#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "include/arraylist.h"

#define INT_MAX 2147483647

const char* HOME = ".";
const char* PARENT = "..";

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

int is_rec(char *path){
 if(strcmp(path,HOME) == 0 || strcmp(path,PARENT) == 0){ 
  return 0;
}
return 1;
}

//NOT IMPLIMENTED, THIS IS JUST A COPY OF COMPRESE
void decompress_file(char* file_path, ArrayList *codebook){
 int file = open(file_path, O_RDONLY);
 if(file == -1){
      //throw error, file doesnt exist
      printf("File does not exist\n");
  return 0;
} 
   // what if text file is bigger
char s[INT_MAX];
while(read(file,s,INT_MAX) > 0){
  printf("%s",s);  
}
printf("\n");

char newPath[200];
   my_strcpy(newPath, newPath);
  strcat(newPath, ".hcz");
int new_file = open(newPath,O_WRONLY | O_CREAT, 0666);

int i;

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
for(i = 0;i<codebook->current_size;i++){
 if(strcmp(t,codebook->token_list[i]->str)==0){
        char str[12];
        sprintf(str, "%d", codebook->token_list[i]->num);
        write(new_file, str, strlen(str));
   }
 }
close(file);
close(new_file);
}

void compress_file(char* file_path, ArrayList *codebook){
 int file = open(file_path, O_RDONLY);
 if(file == -1){
      //throw error, file doesnt exist
      printf("File does not exist\n");
  return 0;
} 
   // what if text file is bigger
char s[INT_MAX];
while(read(file,s,INT_MAX) > 0){
  printf("%s",s);  
}
printf("\n");

char newPath[200];
   my_strcpy(newPath, newPath);
  strcat(newPath, ".hcz");
int new_file = open(newPath,O_WRONLY | O_CREAT, 0666);

int i;

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
for(i = 0;i<codebook->current_size;i++){
 if(strcmp(t,codebook->token_list[i]->str)==0){
        char str[12];
        sprintf(str, "%d", codebook->token_list[i]->num);
        write(new_file, str, strlen(str));
   }
 }
close(file);
close(new_file);
}

/*
1. extract tokens from giant string
for each token:
  convert token w/ huffman
  write new token to .hcz 
*/


// TODO does not fix
void write_file( char *file_path, const char *outname){
  // printf("copying %s into %s", file_name, outname);
 int file = open(file_path,O_RDONLY);
 char buffer[1001];
 int new_file = open(outname,O_WRONLY | O_CREAT, 0666);
   //printf("OUTNAME: %s\n", outname);
 while(read(file,buffer,1000) > 0);
 write(new_file,buffer,strlen(buffer)); 
     printf("BUFFER: %s\n", buffer);
 close(file);
 close(new_file);
}

  void compress_driver(char *path, int recursive){
   DIR* d;
   d = opendir(path);
   struct dirent *dir;
   while((dir = readdir(d)) != NULL){
    if(dir->d_type == DT_DIR){

     if(is_rec(dir->d_name)&& recursive ==1){//check if recursive here

      printf("directory: %s\n", dir->d_name);

      char  fullPath[100];
      my_strcpy(fullPath, path);
      strcat(fullPath,"/");
      strcat(fullPath, dir->d_name);	   

      compress_driver(fullPath);
    }
  } else {
   printf("file: %s:\n", dir->d_name);


   char fullPath[100];
   my_strcpy(fullPath, path);
   strcat(fullPath,"/");
   strcat(fullPath,dir->d_name);
   //strcat(fullPath, ".hcz");
   printf("%s\n",fullPath);
   //write_file(dir->d_name,fullPath);
   compress_file(fullPath);
 }
} 
closedir(d);
} 

  void decompress_driver(char *path, int recursive){
   DIR* d;
   d = opendir(path);
   struct dirent *dir;
   while((dir = readdir(d)) != NULL){
    if(dir->d_type == DT_DIR){

     if(is_rec(dir->d_name)&& recursive ==1){//check if recursive here

      printf("directory: %s\n", dir->d_name);

      char  fullPath[100];
      my_strcpy(fullPath, path);
      strcat(fullPath,"/");
      strcat(fullPath, dir->d_name);     

      decompress_driver(fullPath);
    }
  } else {
   printf("file: %s:\n", dir->d_name);


   char fullPath[100];
   my_strcpy(fullPath, path);
   strcat(fullPath,"/");
   strcat(fullPath,dir->d_name);
   //strcat(fullPath, ".hcz");
   printf("%s\n",fullPath);
   //write_file(dir->d_name,fullPath);
   decompress_file(fullPath);
 }
} 
closedir(d);
} 

int main(){
 char * path = realpath("./test_area", NULL);
 //print_files_in_dir_recursive(path); 
}

