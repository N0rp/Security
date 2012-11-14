/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#define _GNU_SOURCE   /* setresuid */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/wait.h> 
#include <netinet/in.h>

#include "auth.h"

#define FORK 

int enable_fork = 1;

unsigned long get_sp(void) {
   __asm__("movl %esp,%eax");
}

unsigned long get_ret(void) {
   __asm__("movl 4(%ebp),%eax");
}


void handle_con(int sock);

int auth_user(char *user, char *pass);

FILE *f;

void handle_sig(int signal)
{
  wait3(NULL, WNOHANG, NULL); 
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
#ifdef FORK
  int pid;
#endif

  unsigned int clilen;
  int sockfd, newsockfd, portno, on;
  struct sockaddr_in serv_addr, cli_addr;

  f=fopen("/root/ch3.log","a");

  signal(SIGCHLD, handle_sig);

  if (argc < 2) {
    fprintf(stderr,"error: no port provided\n");
    exit(1);
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) 
    error("error: opening socket");

  on = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    error("error: set socket option");

  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    error("error: bind");

  listen(sockfd,5);
  clilen = sizeof(cli_addr);

  while (1) {

//	printf("server: Listening to incoming connections...\n\n");

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0) 
      error("error: accept");
	printf("Handling new socket\n");
#ifdef FORK
	if(enable_fork){
    	pid = fork();
	}else{
		pid = 0;
	}
    if (pid < 0)
      error("error: fork");

    if (pid == 0)  {
      close(sockfd);
#endif
	/* drop privileges */
    handle_con(newsockfd);

#ifdef FORK
      exit(0);
    }
    else close(newsockfd);
#endif


  }

  return 0; /* we never get here */

}

void respond(char *m) 
{
	printf("Responding\n");
	char buf[400];
	//printf("Buffer: %s\n", buf);
	printf("Buffer is at: 0x%lx and code length is: %i\n", buf, strlen(m));
	printf("Returnpointer before copy: 0x%lx\n", get_ret());

	strcpy(buf,m);
	printf("Returnpointer after copy: 0x%lx\n", get_ret());
	printf("Finished Responding->No printf after this one means changed return address\n");
}


void handle_con(int sock)
{
	printf("---------------------\n");
	printf("Handling new connection\n");
  int n, uid, tuid;
  size_t len;
  char msg[512];
  char *user, *pass, *message;
 
  len = sizeof(msg);
  memset(msg, 0, len);

  n = read(sock, msg, len - 1);
  if (n < 0) 
    error("error: reading from socket");
	
//	printf("Message: %s\n", msg);
  /* very simple authentication */
  msg[10] = msg[19] = '\0';

  /* 
   * login with your user id and the password found in your home directory
   * 	e.g.	"inetsec999:XXXXXXXX:<message>"
   * 			"dazur999  :XXXXXXXX:<message>"
   *
   */
  user = msg;
  /* 
   * you will find your password in your home directory on bandit
   *
   * please check twice before sending us email ;-) 
   *
   */
  pass = msg + 11;

  message = msg + 20;
	
	//printf("User: 		%s\n", user);
	//printf("Password:	%s\n", pass);
	//printf("Message:	%s\n", message);


  if ((uid = auth_user(user, pass)) != 0) {

    /* change to inetsec or dazur user on bandit */

    tuid = uid; 
    //if (setresuid(tuid, tuid, tuid) < 0)
      //error("error: setting permissions");
    respond(message);
	printf("Finished Handling Connection\n");
  }  
  else {
	fprintf(f,"%s %s Access denied\n",user,pass);
	fflush(f);
  }
}
