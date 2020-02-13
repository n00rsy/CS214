#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#define INT_MAX 2147483647


int main (int argc, char * argv[]){

  if(argc != 3){
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
    printf("%s\n", c);
    printf("AAAAAA\n");
  }
}
