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

int is_rec(char *path){
 if(strcmp(path,HOME) == 0 || strcmp(path,PARENT) == 0){ 
  return 0;
}
return 1;
}

int compress_file(char* file_path, ArrayList *codebook){
 int file = open(file_path, O_RDONLY);
 if(file == -1){
      //throw error, file doesnt exist
      // printf("File does not exist\n");
  return 0;
} 

   // what if text file is bigger
char buffer[MAXIMUM];
while(read(file,buffer,MAXIMUM) > 0){
  printf("%s",buffer);  
}
printf("\n");

char newPath[200];
   my_strcpy(newPath, file_name);
  strcat(newPath, ".hcz");
int new_file = open(newPath,O_WRONLY | O_CREAT, 0666);

int i;
//for each token{
for(i = 0;i<codebook->current_size;i++){
 //if(strcmp(token,codebook->token_list[i]->str)==0){
        char str[12];
        sprintf(str, "%d", codebook->token_list[i]->num);
        write(new_file, str, strlen(str)), 
   }
}

/*
1. extract tokens from giant string
for each token:
  convert token w/ huffman
  write new token to .hcz 
*/

close(file);
close(new_file);

}

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

  void print_files_in_dir_recursive(char *path){
   DIR* d;
   d = opendir(path);
   struct dirent *dir;
   while((dir = readdir(d)) != NULL){
    if(dir->d_type == DT_DIR){

     if(is_rec(dir->d_name)){//check if recursive here

      printf("directory: %s\n", dir->d_name);

      char  fullPath[100];
      my_strcpy(fullPath, path);
      strcat(fullPath,"/");
      strcat(fullPath, dir->d_name);	   

      print_files_in_dir_recursive(fullPath);
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
   print_file(fullPath);
 }
} 
closedir(d);
} 

int main(){
 char * path = realpath("./test_area", NULL);
 print_files_in_dir_recursive(path); 
}

