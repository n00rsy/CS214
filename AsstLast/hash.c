#include <stdio.h>
#include <openssl/md5.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h> 

int main()
{
  unsigned char c[MD5_DIGEST_LENGTH];
  char *filename = "server.c";
  int i;
  int inFile = open (filename, O_RDONLY);
  MD5_CTX mdContext;
  int bytes;
  unsigned char data[1024];

  if (inFile <0) {
    printf ("%s can't be opened.\n", filename);
    return 0;
  }

  MD5_Init (&mdContext);
  while (bytes = read (inFile,data,1023)){
    MD5_Update (&mdContext, data, bytes);
  }
  MD5_Final (c,&mdContext);
  for(i = 0; i < MD5_DIGEST_LENGTH; i++) printf("%02x", c[i]);
  printf (" %s\n", filename);
  close (inFile);
  return 0;
}

