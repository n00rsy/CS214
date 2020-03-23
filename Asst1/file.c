#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

const char* HOME = ".";
const char* PARENT = "..";

void print_files_in_dir_recursive(const char *path);
int is_rec(const char *path);
int print_file(const char* file_name);
void write_file(const char *path, const char *outname);

int is_rec(const char *path){
   if(strcmp(path,HOME) == 0 || strcmp(path,PARENT) == 0){ 
      return 0;
   }
   return 1;
} 


int print_file(const char* file_name){
   int file = open(file_name, O_RDONLY);
   if(file == -1){
      //throw error, file doesnt exist
      // printf("File does not exist\n");
      return 0;
   } 

   char *buffer = (char*) calloc(1000, 100 *sizeof(char));
   while(read(file,buffer,1000) > 0);
   printf("%s",buffer);  
   free(buffer);
}

void write_file(const char *file_name, const char *outname){
   int file = open(file_name,O_RDONLY);
   char buffer[1000];
   while(read(file,buffer,1000) > 0);
   int new_file = open(outname,O_WRONLY);
   write(new_file,buffer,1000); 
   free(buffer);
}

void print_files_in_dir_recursive(const char *path){
   DIR* d;
   d = opendir(path);
   struct dirent *dir;
   while((dir = readdir(d)) != NULL){
      if(dir->d_type == DT_DIR){
         printf("directory: %s\n", dir->d_name);
         if(is_rec(dir->d_name)){
            print_files_in_dir_recursive(dir->d_name);
         }
      } else {
         printf("file: %s:\n", dir->d_name);
         const char* str = "test";
         write_file(dir->d_name, strcat(str,dir->d_name));
         // print_file(dir->d_name);
      }
   } 
   closedir(d);
} 

int main(){
   print_files_in_dir_recursive("."); 
}

