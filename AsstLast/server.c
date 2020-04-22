#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>


#define PORT "3491"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold
int total_conns = 0;

void sigchld_handler(int s)
{
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while(waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
// must be called from handle_client_connection
void send_client_project(int client_fd, char* project_name){
  // tar the project. output is result.tar.gz
  char buffer[32];
  sprintf(buffer, "tar -zcvf result.tar.gz %s", project_name);
  printf("system call: %s\n", buffer);
  system(buffer);
  // now we have to send result.tar.gz to the client
 int read_fd;
 int write_fd;
 struct stat stat_buf;
 off_t offset = 0;

 /* Open the input file. */
 read_fd = open (project_name, O_RDONLY);
 /* Stat the input file to obtain its size. */
 fstat(read_fd, &stat_buf);
 /* Blast the bytes from one file to the other. */
 sendfile(client_fd, read_fd, &offset, stat_buf.st_size);
 /* Close up. */
 close (read_fd);
  
}

void* handle_client_connection(void* client_fd)  // client file descriptor
{
    
    int client_descriptor = *(int*) client_fd;

    // keep handling the client in this thread
    int success = send(client_descriptor, "dear client: you have your own thread!", 100, 0);
    if(success < 0){
      printf("error in sending client data\n");
    }
    int BUFFER_LEN = 100;
    int num_bytes;
    send_client_project(client_descriptor, "testproject");

    // handle incoming client requests
    while(1){
      // buffer for data sent to us from client
      char buffer[BUFFER_LEN];
      int num_bytes;
      size_t len;

      //recieve commands from client from here
      // commands should be stored in command.txt file with first line being command, and second lind being project name
      if((num_bytes = recv(client_descriptor, buffer, BUFFER_LEN - 1, 0) < 0)){
        if(strcmp(buffer,"checkout") == 0){
          // if the user wants to check out a project name send them a project
          // send_client_project(client_descriptor, buffer)
        }
        	printf("nothing from client, error");
        	continue;
      } else {
	// end this thread + client connection
	break;
      }
    
      buffer[BUFFER_LEN] = '\0';
      printf("server: received %s\n", buffer);
    }

    return NULL;
}



int main(void)
{
  int sockfd; // server will listen on sock_fd
  int new_fd;  // each new client connection has its own file descriptor
  struct addrinfo hints; // getaddrinfo will fill this struct out
  struct addrinfo *servinfo; // information about the server
  struct addrinfo *p; // ptr to traverse linkedlist (see struct addrinfo format )

  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;

  // signal handler
  struct sigaction sa;

  int yes = 1;
  
  char s[INET6_ADDRSTRLEN];

  int rv;

  // clear out the hints struct so we can put stuff in it
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, unspecificied
  hints.ai_socktype = SOCK_STREAM; // SOCK_STREAM is TCP, we will use it for this project
  hints.ai_flags = AI_PASSIVE; // AI_PASSIVE means that we will use localhost to bind

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
	    p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
	  sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo); // all done with this structure

  if (p == NULL)  {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  while(1) {  // main accept() loop
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    total_conns += 1;
    // inet_ntop converts the network address to a normal string for us to read
    inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    printf("server: got connection from %s\n", s);
    printf("total connections now: %d\n", total_conns);
    send(new_fd, "Thank you for connecting. We are sending you to a new thread", 100, 0);
    pthread_t client_thread;
    pthread_create(&client_thread, NULL, handle_client_connection, (void*) &new_fd);
    printf("created new thread for client\n");
  }

  return 0;
}
