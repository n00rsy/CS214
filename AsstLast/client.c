#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h>
#include <openssl/md5.h>

#define PORT "3491" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

const int buffer_size = 100;

typedef struct file_node {
   int versionNum;
   char* filePath;
   char *hash;//prob change this data type

   struct file_node * next;
} fileNode;

typedef struct manifest_struct {
   int versionNum;
   fileNode * head;

} manifestStruct;

void printManifest(manifestStruct * man){
  printf("Manifest Version Number: %d\n\n", man->versionNum);

  fileNode * ptr = man->head;
  if(ptr==NULL){
    printf("Manifest empty.\n");
    return;
  }

  while(ptr!=NULL){
    printf("File version num: %d\tpath: '%s'\thash: '%s'\n",
    ptr->versionNum, ptr->filePath, ptr->hash);
    ptr=ptr->next;
  }

}

void freeManifest(manifestStruct * man){
  fileNode * ptr = man->head;
  fileNode * prev;

  while(ptr!=NULL){
    prev = ptr;
    ptr=ptr->next;
    free(prev->filePath);
    free(prev->hash);
    free(prev);
  }
  free(ptr->filePath);
  free(ptr->hash);
  free(ptr);
}

char * hash(char * filePath){
   // printf("hashing %s\n", filePath);
   // char c[MD5_DIGEST_LENGTH];
    char * cOut = malloc((50)*sizeof(char));
     unsigned char c[MD5_DIGEST_LENGTH];
  int i;
  int inFile = open (filePath, O_RDONLY);
  MD5_CTX mdContext;
  int bytes;
  unsigned char data[1024];

  if (inFile <0) {
    printf ("%s can't be opened.\n", filePath);
    return NULL;
  }

  MD5_Init (&mdContext);
  while (bytes = read (inFile,data,1023)){
    MD5_Update (&mdContext, data, bytes);
  }
  MD5_Final (c,&mdContext);
  for(i = 0; i < MD5_DIGEST_LENGTH; i++){
//	 printf("%02x", c[i]);
	if(i==0){
	sprintf(cOut,"%02x", c[i]);
	}
	else{
	sprintf(cOut+strlen(cOut),"%02x", c[i]);
	}
  }
 // cOut[MD5_DIGEST_LENGTH] = '\0';
//  printf("\n\n FINAL HASH: %s\n", cOut);
  close (inFile);
  return cOut;
}

//parses line in format <version num> \t <path/name> \t <hash> \t
fileNode * parseLine(char * line){
  fileNode * fNode = malloc(sizeof(fileNode));

  int count = 0;
char *ptr = line;
while((ptr = strchr(ptr, '\t')) != NULL) {
    count++;
    ptr++;
}
if(count!=3){
  printf("Found corrupted line in manifest: %s\n", line);
  return NULL;
}

  char * token = strtok(line, "\t");
  fNode->versionNum = atoi(token);
  //printf("file version: %d\n", fNode->versionNum);

  token = strtok(NULL, "\t");
  int size = strlen(token)+1;
  fNode->filePath = malloc(size*sizeof(char));
  strcpy(fNode->filePath, token);
  //printf("file path: %s\n", fNode->filePath);

  token = strtok(NULL, "\t");
  size = strlen(token)+1;
  fNode->hash = malloc(size*sizeof(char));
  strcpy(fNode->hash, token);
  //printf("hash: %s\n\n", fNode->hash);
  return fNode;
}

//returns pointer to struct containing llist of files
manifestStruct * readManifest(char * manifestPath){

int manifestFD = open(manifestPath,O_RDWR);
  if(manifestFD<0){
   printf(".Manifest not found\n");
   return NULL;
  }

  manifestStruct * man = malloc(sizeof(manifestStruct));
  man->head = NULL;
  int readLen;
  char buffer[buffer_size];
  char fullBuffer[buffer_size*2];
  fullBuffer[0] = '\0';
  int count = 0;
  buffer[buffer_size]='\0';

  while(readLen = read(manifestFD, buffer, buffer_size-1)){
    buffer[readLen]='\0';
    //printf("initial buffer: %s\n", buffer);
    //handle file version on top of file
    if(count ==0){
      char * p = &buffer[0];
      while(*p!='\n'){
        if(*p=='\0'){
          printf("Manifest formatted incorrectly.\n");
          free(man);
          return NULL;
        }
        p++;
      }
      *p='\0';
      man->versionNum = atoi(buffer);
      int i = 0;
      p++;
      while(*p!='\0'){
        buffer[i] = *p;
        p++;
        i++;
      }
      buffer[i] = '\0';

      count++;
      //printf("First buffer: %s\n", buffer);
    }

    strcat(fullBuffer,buffer);
    char * p2 = &fullBuffer[0];
    char * p1;
    int shouldbreak = 0;
    
    while(*p2!='\0'){
      p1=p2;
      while(*p2!='\n' && *p2!='\0'){
        p2++;
      }

      int i;
      switch(*p2){
        case '\0':
          i=0;
          while(*p1!='\0'){
            fullBuffer[i] = *p1;
            i++;
            p1++;
          }
          fullBuffer[i] = '\0';
          //printf("\n\nleftover buffer: '%s'\t%c\n", fullBuffer, *(p2-1));
          break;
        case '\n':
        *p2='\0';
        //printf("Parsing %s\n", p1);
        fileNode * new = parseLine(p1);
        if(new!=NULL){
        //insert new node at end of llist
        if(man->head ==NULL){
          man->head = new;
        }
        else{
          fileNode * last = man->head;
          while(last->next !=NULL){
            last = last->next;
          }
          last->next = new;
        }
      }
        *p2='\n';
          p2++;
          break;
      }
    }
  }
  close(manifestFD);
  return man;
}

//completely overwrites .manifest with data from man struct passed in
int writeManifest(manifestStruct * man, char * manifestPath){
  int manifest = open(manifestPath,O_WRONLY | O_CREAT | O_APPEND |O_TRUNC, 0666);
  if(manifest<0){
    printf("Failed to open .manifest\n");
    return 0;
  }

  char str[12];
  //write manifest version number
  sprintf(str, "%d", man->versionNum);
  write(manifest,str,strlen(str));

  fileNode * ptr = man->head;
  while(ptr!=NULL){
    write(manifest, "\n",1);
    //write file version number
    sprintf(str, "%d", ptr->versionNum);
    write(manifest,str,strlen(str));
    write(manifest, "\t",1);

    //write path
    write(manifest, ptr->filePath, strlen(ptr->filePath));
    write(manifest, "\t",1);

    //write hash
    write(manifest, ptr->hash, strlen(ptr->hash));
    write(manifest, "\t",1);
    ptr=ptr->next;
  }
  write(manifest, "\n",1);
  close(manifest);
}


fileNode * createFileNodeFromPath(char * projectName, char * filePath){
  
  char * fullPath = malloc(sizeof(char)*(strlen(projectName)+strlen(filePath)+5));
  strcpy(fullPath, projectName);
  strcat(fullPath, "/");
  strcat(fullPath, filePath);
  
  fileNode * fNode = malloc(sizeof(fileNode));
  if(fNode ==NULL){
    return NULL;
  }
  fNode->versionNum = 1;
  fNode->filePath = filePath;
  fNode->hash =hash(fullPath);
  free(fullPath);
  return fNode;
}

int addFile(char * projectName, char * filePath){
 //check if project exists.
 DIR* d;
 d = opendir(projectName);
 if(d==NULL){
   printf("Project does not exist.\n");
   return 0;
 }
  int mainfestPathLen = strlen(projectName)+12;
  char * manifestPath = (char *)malloc(1000*sizeof(char));
  strcpy(manifestPath, projectName);
  strcat(manifestPath, "/.Manifest");
   //check if file is already in .Manifest
  manifestStruct * man = readManifest(manifestPath);
  if(man ==NULL){
    printf("Unable to open manifest or corrupted manifest\n");
    return 0;
  }
  //add node to end of llist
  fileNode * ptr = man->head;
  fileNode * last;
  while(ptr!=NULL){
    if(strcmp(filePath, ptr->filePath)==0){
      printf("File %s already in Manifest.\n", filePath);
      return 0;
    }
    if(ptr->next ==NULL){
      last = ptr;
    }
    ptr=ptr->next;
  }
  last->next = createFileNodeFromPath(projectName,filePath);

  printf("Printing manifest\n");
  printManifest(man);
  writeManifest(man,manifestPath);
  free(manifestPath);
  return 1;
}

int removeFile(char * projectName, char * filePath){
 //check if project exists.
 DIR* d;
 d = opendir(projectName);
 if(d==NULL){
   printf("Project does not exist.\n");
   return 0;
 }
  int mainfestPathLen = strlen(projectName)+12;
  char * manifestPath = (char *)malloc(1000*sizeof(char));
  strcpy(manifestPath, projectName);
  strcat(manifestPath, "/.Manifest");
   //check if file is already in .Manifest
  manifestStruct * man = readManifest(manifestPath);
  if(man ==NULL){
    printf("Unable to open manifest or corrupted manifest\n");
    return 0;
  }

  //remove node from llist
  fileNode * ptr = man->head;
  fileNode * prev;
  
  if(ptr !=NULL && strcmp(ptr->filePath, filePath)==0){
    man->head = ptr->next;
    free(ptr);
    return 1;
  }
  while(ptr!=NULL && strcmp(ptr->filePath, filePath)!=0){
    prev = ptr;
    ptr = ptr->next;
  }
  if (ptr == NULL){
    printf("File %s not found in Manifest.\n", filePath);
    return 0;
  }
  prev->next = ptr->next; 
  free(ptr);

  printManifest(man);
  writeManifest(man,manifestPath);
  free(manifestPath);
  return 1;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{

  addFile("test", "test.txt");
 hash("test/test.txt");
  int sockfd, numbytes;  
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  if (argc != 2) {
    fprintf(stderr,"usage: client hostname\n");
    exit(1);
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
	    p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure

  // client loop
  while(1){
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
      perror("recv");
    } else {
      buf[numbytes] = '\0';
      printf("client: received from server '%s'\n",buf);

      char text[10];
      printf("Please enter some text to send to the server");
      scanf("%s",text);

      send(sockfd, text, 10,0);

    }
  }
  close(sockfd);
  return 0;
}

