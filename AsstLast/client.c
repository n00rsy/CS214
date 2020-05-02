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

// writes to .Configure file
void configure(char* host, char* port){
  if( access(".configure", F_OK ) != -1 ) {
    printf("found .configure, rewriting it!\n");
  } else {
    printf("creating .configure file\n");
  }

  // create configure file
  int configure_fd = open(".configure", O_CREAT | O_RDWR ,0666);

  int len = strlen(host) + strlen(port) + 1;
  char config_buf[len];
  sprintf(config_buf, "%s:%s",host,port);
  int writtenbytes = write(configure_fd,config_buf,len);
  if(writtenbytes <= 0){
   printf("failed to create a configure file");
  } else {
    printf("wrote %d bytes to configure file\n", writtenbytes);
  }
}

const int buffer_size = 100;

typedef struct file_node {
  char versionNum;
  char* filePath;
  char *hash;//prob change this data type

  struct file_node * next;
} fileNode;

typedef struct dotfile_struct {
  int versionNum;
  fileNode * head;

} manifestStruct;

void printFileNode(fileNode * fNode){
  printf("File version num: %d\tpath: '%s'\thash: '%s'\n",
      fNode->versionNum, fNode->filePath, fNode->hash);
}

void printManifest(manifestStruct * man){
  printf("Manifest Version Number: %d\n\n", man->versionNum);

  fileNode * ptr = man->head;
  if(ptr==NULL){
    printf("Manifest empty.\n");
    return;
  }

  while(ptr!=NULL){
    printFileNode(ptr);
    ptr=ptr->next;
  }

}

void freeFileNode(fileNode * fNode){
  //free(fNode ->filePath);
  //free(fNode->hash);
  free(fNode);
}

void freeManifest(manifestStruct * man){
  fileNode * ptr = man->head;
  fileNode * prev;

  while(ptr!=NULL){
    prev = ptr;
    ptr=ptr->next;
    freeFileNode(prev);
  }
  freeFileNode(ptr);
}


char * hash(char * filePath){
  /*
     char * c = malloc((MD5_DIGEST_LENGTH+1)*sizeof(char));

     c[MD5_DIGEST_LENGTH] = '\0';
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
     for(i = 0; i < MD5_DIGEST_LENGTH; i++) printf("%02x", c[i]);
     printf (" %s\n", filePath);
     close (inFile);
     if(strcmp(c, "")==0){
     c = "<empty file>";
     }
     return c;
   */

  char * ye = malloc(10*sizeof(char));
  ye= "ye";
  return ye;
}

char * getManifestPath(char * projectName){
  int mainfestPathLen = strlen(projectName)+12;
  char * manifestPath = (char *)malloc(mainfestPathLen*sizeof(char));
  strcpy(manifestPath, projectName);
  strcat(manifestPath, "/.Manifest");
  return manifestPath;
}

char * getUpdatePath(char * projectName){
  int mainfestPathLen = strlen(projectName)+12;
  char * manifestPath = (char *)malloc(mainfestPathLen*sizeof(char));
  strcpy(manifestPath, projectName);
  strcat(manifestPath, "/.Update");
  return manifestPath;
}

char * getConflictPath(char * projectName){
  int mainfestPathLen = strlen(projectName)+12;
  char * manifestPath = (char *)malloc(mainfestPathLen*sizeof(char));
  strcpy(manifestPath, projectName);
  strcat(manifestPath, "/.Conflict");
  return manifestPath;
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

int writeConflict(fileNode * fNode, char * projectName){

}

int writeUpdate(fileNode * fNode, char * projectName){

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
  fNode->hash = hash(fullPath);
  fNode->next = NULL;
  free(fullPath);
  return fNode;
}


int addFile(char * projectName, char * filePath){
  //check if project exists on client side
  DIR* d;
  d = opendir(projectName);
  if(d==NULL){
    printf("Project does not exist.\n");
    return 0;
  }

  char * manifestPath =getManifestPath(projectName);
  manifestStruct * man = readManifest(manifestPath);

  if(man ==NULL){
    printf("Unable to open manifest or corrupted manifest\n");
    return 0;
  }
  printManifest(man);
  printf("\n\n");
  //add node to llist w/ alphabetical sort

  fileNode * new = createFileNodeFromPath(projectName,filePath);
  //printFileNode(new);
  if(new == NULL){
    printf("Error detected. Unable to add file.\n");
    free(manifestPath);
    freeFileNode(new);
    return 0;
  }
  fileNode * ptr = man->head;
  fileNode * prev;

  if(strcmp(new->filePath, ptr->filePath)==0){
    printf("File %s %s already in Manifest.\n", new->filePath, ptr->filePath);
    free(manifestPath);
    freeFileNode(new);
    return 0;
  }

  if(ptr==NULL || strcmp(new->filePath, ptr->filePath)<=0){
    new->next = man->head;
    man->head = new;
  }
  else{
    while(ptr->next!=NULL && strcmp(new->filePath, ptr->filePath)>0){
      ptr = ptr->next;
    }
    if(strcmp(new->filePath, ptr->filePath)==0){
      printf("File %s already in Manifest.\n", filePath);
      free(manifestPath);
      freeFileNode(new);
      return 0;
    }

    new->next = ptr->next;
    ptr->next = new;
  }
  /*
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
   */
  printManifest(man);
  writeManifest(man,manifestPath);
  free(manifestPath);
  return 1;
}

int removeFile(char * projectName, char * filePath){
  //check if project exists on client side
  DIR* d;
  d = opendir(projectName);
  if(d==NULL){
    printf("Project does not exist.\n");
    return 0;
  }

  char * manifestPath = getManifestPath(projectName);
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

  //printManifest(man);
  writeManifest(man,manifestPath);
  free(manifestPath);
  return 1;
}

int update(char * projectName){
  //check if project exists on server
  //check if client can communicate with server
  DIR* d;
  d = opendir(projectName);
  if(d==NULL){
    printf("Project does not exist.\n");
    return 0;
  }

  char * manifestPath = getManifestPath(projectName);
  manifestStruct * clientManifest = readManifest(manifestPath);

  if(clientManifest ==NULL){
    printf("Unable to open manifest or corrupted manifest\n");
    return 0;
  }
  //get manifest from server
  manifestStruct * serverManifest = readManifest("serverTest/.Manifest");

  //full success case
  if(serverManifest->versionNum == clientManifest->versionNum){
    //write blank .Update and end
    char * updatePath = getUpdatePath(projectName);
    int update = open(updatePath,O_WRONLY | O_CREAT | O_APPEND |O_TRUNC, 0666);
    free(updatePath);
    close(update);
    return 1;
  }
  //compare .Manifest content
  fileNode * clientPtr = clientManifest->head;
  fileNode * serverPtr = serverManifest->head;

  char * conflictPath = getConflictPath(projectName);
  char * updatePath = getUpdatePath(projectName);

  while(clientPtr!=NULL){
    int found = 0;
    char * liveHash = hash(clientPtr->filePath);
    while(serverPtr!=NULL){
      if(strcmp(clientPtr->filePath, serverPtr->filePath)==0){
	found = 1;
	//conflict
	if(strcmp(serverPtr->hash, clientPtr->hash)!=0 &&
	    strcmp(liveHash,clientPtr->hash)!=0){
	  printf("C %s %s", clientPtr->filePath, liveHash);
	  continue;
	}
	//modify code
	if(clientPtr->versionNum!=serverPtr->versionNum &&
	    strcmp(serverPtr->hash, clientPtr->hash)!=0 &&
	    strcmp(clientPtr->hash, liveHash)==0){

	}
      }
      serverPtr = serverPtr->next;
    }
    //delete code
    if(found == 0){

    }

    serverPtr = serverManifest->head;
    clientPtr=clientPtr->next;
    free(liveHash);
  }

  //bad initial design. have to do second pass for add code
  clientPtr = clientManifest->head;
  serverPtr = serverManifest->head;

  while(serverPtr!=NULL){
    int found = 0;
    while(clientPtr!=NULL){
      if(strcmp(clientPtr->filePath, serverPtr->filePath)==0){
	found = 1;
      }

      clientPtr=clientPtr->next;
    }
    //add code
    if(found ==0){

    }
    clientPtr = clientManifest->head;
    serverPtr = serverPtr->next;
  }


  free(updatePath);
  free(conflictPath);
  free(manifestPath);
}


// basically git pull
int upgrade(char * projectName){
  //check if server can be contacted
  //check if project exists on server

  char * updatePath = getUpdatePath(projectName);
  int update = open(updatePath, O_RDONLY);
  if(update <0){
    printf("Need to run update or no updates\n");
    free(updatePath);
    return 0;
  }
  char * conflictPath = getConflictPath(projectName);
  int conflict = open(conflictPath, O_RDONLY);
  if(conflict >0){
    printf("Conficts exist. Cannot update.\n");
    free(updatePath);
    free(conflictPath);
    close(conflict);
    return 0;
  }

  //do upgrades 


  close(update);
  free(updatePath);
  free(conflictPath);
}
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// is there .configure file?

int doesConfigureExist(){
  if(access(".configure", F_OK ) != -1 ) {
    return 1;
  } else {
    return 0;
  }
}

int main(int argc, char *argv[])
{

  if (argc < 2) {
    fprintf(stderr,"not enough arguments usage => ./WTF `command` \n");
    exit(1);
  }

  char *op = argv[1]; 
  if(strcmp(op,"configure") == 0){
    if(argc < 4){
	printf("not enough arguments usage => ./WTF configure `hostname` `port` \n");
	return 1;
    }
    char *host = argv[2];
    char *port = argv[3];

    configure(host,port);
    return 1;
  } 

  // addFile("test", "test.txt");
  // hash("test/test.txt");
  int sockfd, numbytes;  
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];


  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  // open configure file
  int configure_fd = open(".configure", O_RDONLY,0666);

  off_t fsize;
  fsize = lseek(configure_fd,0,SEEK_END);
  lseek(configure_fd,0,0);
  char config_buff[fsize];
  int bytes_read = read(configure_fd, config_buff, fsize);
  if(bytes_read <= 0){
    printf("error in reading configure file\n");
    return 1;
  }
  char *hostname; 
  char *port;
  //read buffer to get hostname and port
  char * curLine = config_buff;
  int cnt = 0;
  printf("config buff : %s\n", config_buff);

  char *configptr;
  configptr = strtok(config_buff, ":");
  hostname = configptr;
  configptr = strtok(NULL, ":");
  port = configptr;
  // ok now we got host and port from .config
  if ((rv = getaddrinfo(hostname, port , &hints, &servinfo)) != 0) {
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


  // parse command
  if(strcmp(op,"checkout") == 0){
    if(doesConfigureExist() == 0){
      printf("No .configure file found. Please run configure\n");
      return 1;
    }
    if(argc < 3){
      printf("no project name given usage -> ./WTF checkout `projectname`");
      return 1;
    }
    // send client cmd and project name in the following format checkout:projectname
    char *projname = argv[2];
    printf("getting %s from server...\n", projname);
    char checkout_buff[strlen("checkout") + strlen(projname)];
    sprintf(checkout_buff,"checkout:%s",projname);
    // now the cmd string is ready, lets send it to the server to get our projetc
    send(sockfd,checkout_buff,strlen(checkout_buff),0);

    // server response, file parsing and untarring done here
    if((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) != 1 ){
      // copy buf into buffercpy
      char *buffercpy = malloc(sizeof(char) * MAXDATASIZE);
      strcpy(buffercpy,buf);
      char *tokenptr;
      tokenptr = strtok(buffercpy, ":");
      tokenptr = strtok(NULL, ":");
      printf("client: project length in bytes is %s \n", tokenptr);
      off_t file_size = atoi(tokenptr);
      char *file_buffer = (char*) malloc(sizeof(char) * file_size);
      int bytes_recv = read(sockfd,file_buffer, file_size);
      printf("client: bytes recieved from server %d\n", bytes_recv);

      // write the bytes into a file
      char *untar = "tar -zxvf client.tar.gz";
      int finalfd = open("client.tar.gz", O_CREAT | O_RDWR ,0666);
      int writtenbytes = write(finalfd,file_buffer,file_size);
      perror(strerror(errno));
      printf("client: bytes written to file %d\n", writtenbytes);
      printf("client: untarring project");
      system(untar); 
      system("rm -rf client.tar.gz");
      printf("checkout was a sucess\n");
    } else {
	printf("the server could not find the project \n");
    }
    close(sockfd);
    printf("checkout\n");
  } 
  if(strcmp(op,"update") == 0){
  } 
  if(strcmp(op,"upgrade") == 0){
  } 
  if(strcmp(op,"commit") == 0){
  } 
  if(strcmp(op,"push") == 0){
  } 
  if(strcmp(op,"push") == 0){
  } 

  // wait to recieve the response.
}
