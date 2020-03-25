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

void print_files_in_dir_recursive( char *path);
int is_rec( char *path);
int print_file(char* file_name);
void write_file(char *path, const char *outname);

int is_rec(char *path){
   if(strcmp(path,HOME) == 0 || strcmp(path,PARENT) == 0){ 
      return 0;
   }
   return 1;
}

int print_file( char* file_name){
   int file = open(file_name, O_RDONLY);
   if(file == -1){
      //throw error, file doesnt exist
      // printf("File does not exist\n");
      return 0;
   } 

   // what if text file is bigger
   char *buffer = (char*) calloc(1000, 100 *sizeof(char));
   while(read(file,buffer,1000) > 0);
  // printf("%s",buffer);  
   free(buffer);
}

// TODO does not fix
void write_file( char *file_name, const char *outname){
  // printf("copying %s into %s", file_name, outname);
   int file = open(file_name,O_RDONLY);
   char buffer[1001];
   int new_file = open(outname,O_WRONLY | O_CREAT | O_APPEND, 0666);
   //printf("OUTNAME: %s\n", outname);
   while(read(file,buffer,1000) > 0);
      write(new_file,buffer,1000); 
     // printf("BUFFER: %s\n", buffer);
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

         if(is_rec(dir->d_name)){

            printf("directory: %s\n", dir->d_name);
	    
	   char  fullPath[100];
 	   my_strcpy(fullPath, path);
	   strcat(fullPath,"/");
	   strcat(fullPath, dir->d_name);	   
	
            print_files_in_dir_recursive(fullPath);
         }
      } else {
         printf("file: %s:\n", dir->d_name);
          // char* path = realpath(dir->d_name, );

//	  if(path == NULL){
  //           printf("cannot find file with name[%s]\n", path);

           // printf("path[%s]\n", path);
              
	   char fullPath[100];
 	   my_strcpy(fullPath, path);
	   strcat(fullPath,"/");
	   strcat(fullPath,dir->d_name);
	   strcat(fullPath, ".hcz");
	   printf("%s\n",fullPath);
         write_file(dir->d_name,fullPath);
         print_file(dir->d_name);
      }
   } 
   closedir(d);
} 

int main(){

   char * path = realpath(".", NULL);
   print_files_in_dir_recursive(path); 
}

