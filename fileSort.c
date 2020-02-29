#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#define INT_MAX 2147483647


typedef union token_t {
   char* str;
   int num;
} token;

typedef struct ArrayList_t{
   // 0 if string, anything else if num
   int is_num;
   // total size of the Array allocated
   size_t total_size;
   // number of tokens actually used in the array
   size_t current_size;
   // list of tokens
   token** token_list;
} ArrayList;

ArrayList *array;

void convertArrayToInts(){
  int i;
  for(i=0;i<array->current_size;i++){
    token * current = array->token_list[i];
    int a = atoi(current->str);
    current->num = a;
  }
}


int intcmp(void* a, void *b){
 return (*(int *)a )- (*(int *)b);
}

int strcmp(void *first, void *second){

   char * f = (char *)first;
   char * s = (char *)second;
   while (*f != '\0' && *s!= '\0'  && *f == *s) {
      f++;
      s++;
   }
   return *f - *s;
}


int intcmp_token(void* a, void *b){

 int first = ((token * )a)->num;
 int second = ((token *)b)->num;

 return first-second;
}

int strcmp_token(void *first, void *second){

   char * f = ((token *)first)->str;
   char * s = ((token *)second)->str;
   while (*f != '\0' && *s!= '\0'  && *f == *s) {
      f++;
      s++;
   }
   return *f - *s;
}



int insertionSort(void* toSort, int (*comparator)(void*, void*));
int quickSort(void* toSort, int (*comparator)(void*, void*));


void pnum(int num){
   printf("%d\n",num);
}

void pstr(char* str){
   printf("'%s'\n",str);
}

// print string without new line, i need cuz ilab wont let me scroll all the way back
// so I cant see the initial printfs
void pstrno( char* str){
   printf("%s ",str);
}
void add(token*);
/*
   make a method to declare a new arraylist
   make another method for add ( handle overlfow)

   1) open file put into char array
   2) determine if num or string parse char array into ArrayList
   3) sort


   1) 2 pointers until comma
   2) count len of token
   3) allocate memory for token
   4) add to data structure


*/

size_t strLength(char * s){

   int c = 0;
   while(s[c] != '\0'){
      c++;
   }
   return c;
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

int isSpecialChar(char c){
   if(c==' ' || c==',' || c=='\t' || c== '\n' || c == EOF){
      return 1;
   }
   return 0;
}

void strToArrayList(char * s){

   int p1 = 0;
   int p2 = 0;
   while(s[p2] != '\0'){
      //scan token
      while(s[p2] != ','&& s[p2] != '\0'&&  s[p2]!= ' ' && s[p2]!= ',' && s[p2]!= '\t' && s[p2]!= '\n' ){
         p2++;
      }
      //extract token and put in data structure
      char *str = substring( s, p1 + 1, p2 - p1);
      // printf("%s\n", str);
      token *t = malloc(sizeof(token));
      t->str = str;
      add(t);

       while(s[p2] != ','&& s[p2] != '\0'){
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

void add(token *t){
   if(array->current_size >= array->total_size){
      //make arrray bigger		
      // should we worry about freeing memory of old list? - RA 
      // pstr("COPYING");
      // pnum(array->current_size);
      token** newList = malloc(sizeof(token)*array->total_size*2);
      int i;
      for(i=0; i< array->total_size;i++){
         newList[i] = malloc(sizeof(token));
         size_t strLen = strLength(array->token_list[i]->str);
         newList[i]-> str = malloc(sizeof(char)*strLen+1);
         newList[i]->str[strLen] = '\0';
         char j;
         for(j=0;array->token_list[i]->str[j] != '\0';++j){
            newList[i]->str[j] = array->token_list[i]->str[j];
         }
      }
      // this fixes junk files in the bieginning
      array->token_list = newList;
      array->total_size*=2;
   }

   array->token_list[array->current_size] = malloc(sizeof(token));
   array->token_list[array->current_size] = t;
   array->current_size+=1;

}

int insertionSort(void* toSort, int (*comparator)(void*, void*)){
   int i, j;
   token *key;
   for(i=1;i< array->current_size;i++){
	key = (array->token_list)[i];
	j=i-1;
	
	while(j>=0 && comparator(&((array->token_list)[j]), &(key))>0){
	
	   (array->token_list)[j+1] = array->token_list[j];
	   j=j-1;

	}
	array->token_list[j+1] = key;
   } 
}

int quickSort(void* toSort, int (*comparator)(void*, void*)){



}
void init(size_t size){
   array = malloc(sizeof(ArrayList));
   array->total_size = size;
   array->current_size = 0;
   array->is_num = 0;
   // create a list of tokens
   token** list_of_tokens = malloc(sizeof(token) * size);
   int i; 
   for(i = 0; i < array->total_size; i++){
      list_of_tokens[i] = malloc(sizeof(token));
   }
   array->token_list = list_of_tokens;
}

void print_array(){
   int i;
 // printf("%d\n", array->current_size);
   for(i = 0; i < array->current_size; i++){
      if(array->is_num){
         pnum(atoi(array->token_list[i]->str));
      } else {
         pstr(array->token_list[i]->str);
      }
   }
}

// print first n elements of array
void printn(int n){
   int i;
   for(i = 0; i < n; i++){
      if(array->is_num){
         pnum(atoi(array->token_list[i]->str));
      } else {
         // pstr(array->token_list[i]->str);
         pstr(array->token_list[i]->str);
      }
   }
}

int checkIfInt(char * t){
   if(t[0]>=48 && t[0]<=57){
      return 1;
   }
   return 0;

}

int main (int argc, char * argv[]){
  init(100);
      /*
      int i;
      for(i=0;i<1000;i++){
      token *test = malloc(sizeof(token));
      if(i%2==0){
      test->str = "X";
      }
      else if(i%3==0){
      test->str = "X";
      }
      else{
      test->str = "X";
      }	
      add(test);
      }   
      print_array();
      */
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
   
   

   char *c = (char *) calloc(INT_MAX, sizeof(char));
   while(read(file,c,INT_MAX)>0){
   }
   strToArrayList(c);
   //char *first = "abc";
   //char *second = "xyz";
   int a = 4;
   int b =10;
  // pnum(intcmp(&a,&b));
   
   print_array();
   printf("\n\n\n");

   char * insert = "-i";
   char * quick = "-q";

   int (*cmp_pointer)(void *, void *);
   if(checkIfInt(array->token_list[0]->str)==0){ //not an int
	cmp_pointer = &strcmp_token;
   }
   else{
	convertArrayToInts();
	cmp_pointer = &intcmp_token;
   }

   if(strcmp(argv[1], insert) == 0){
      insertionSort((void*)&array, cmp_pointer);
   }
   else if (strcmp(argv[1], quick) == 0){
      quickSort((void*)&array, cmp_pointer);
   }
   else{
      printf("Non-valid sort input\n");
      return 1;
   }
   print_array();
   
}
