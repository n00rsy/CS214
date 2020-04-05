#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

#include "include/arraylist.h"
#include "include/huffman.h"
#include "include/tokenize.h"

const int RECURSIVE = 0;
const int BUILDCODEBOOK = 1;
const int COMPRESS = 2;
const int DECOMPRESS = 3;

void buildCodebook(int recursive){
 
}

void compress(int recursive){
/*
char * fileName = "HuffmanCodebook";
readCodebookFromFile)(fileName);
replace each instance of token with code
2. traverse files and output compression based on codebook
*/

}

void decompress(int recursive){

  /*
char * fileName = "HuffmanCodebook";
readCodebookFromFile)(fileName);
   replace coded version with uncoded version
  2. traverse files and output decompression based on codebook
  */

}

void (*methods[])(int) = {buildCodebook, compress, decompress};

int main (int argc, char * argv[]){
  //evaluating args. I did this kind of wrong initially and now its way more complicated than it has to be. it works tho
  int i;
  int j;

  int flagValues[4] = {0,0,0,0};
  const char flags[4][3] = {"-R","-b","-c","-d"};
  int numFlags = 0;

  for(i = 0; i<argc;i++){
    for(j = 0; j<4;j++){
      if(strcmp(argv[i],flags[j])==0){
        flagValues[j]=1;
        numFlags++;
      }
    }
  }

  for(i=0;i<4;i++){
    printf("%s: %d\n", flags[i],flagValues[i]);
  }

  if((flagValues[COMPRESS]+flagValues[DECOMPRESS]+flagValues[BUILDCODEBOOK])!=1){
    printf("incorrect args\n");
    exit(1);
  }

  char filePath[100];
  if(argc>(1+numFlags)){
  strcpy(filePath, argv[1+numFlags]);
  printf("File path: %s\n", filePath);
  }
  else{
    printf("No path provided\n");
    exit(1);
  }

  for(int i =1;i<3;i++){
    if(flagValues[i]==1){
      (*methods[i])(flagValues[RECURSIVE]);
    }
  }

}
