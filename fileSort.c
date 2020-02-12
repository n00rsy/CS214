#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

int main (int argc, char * argv[]){

  if(argc!=3){
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
}
