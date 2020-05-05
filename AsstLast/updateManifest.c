

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
    printf("Failed to open .manifest: %s\n", manifestPath);
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

char * hash(char * filePath){
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
  printf(" %s\n", filePath);
  close (inFile);
  if(strcmp(c, "")==0){
    c = "<empty file>";
  }
  return c;
}

int updateManifest(char * pathToNewManifest){
  manifestStruct * newManifest = readManifest(pathToNewManifest);
  if(newManifest==NULL){
    printf("error\n");
    return 0;
  }
  newManifest->versionNum = newManifest->versionNum+1;
  fileNode * ptr = newManifest->head;

  while(ptr!=NULL){
    char * liveHash = hash(ptr->filePath);//NEED TO GET FILE PATH ON SERVER!!!
    if(strcmp(liveHash, ptr->hash)!=0){
      ptr->versionNum = ptr->versionNum+1;
      ptr->hash = liveHash;
    }
    ptr=ptr->next;
  }
  writeManifest(newManifest);
  freeManifest(newManifest);
}