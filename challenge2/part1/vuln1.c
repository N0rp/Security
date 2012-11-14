/*
 ***  Copyright notice: This file is copyright protected. iSecLab
 ***  grants registered students the right to use and study
 ***  the code. However, participants are not granted the right to distribute
 ***  this code to third parties.
 ***     
 **/
#include <stdio.h>
#include <string.h>
#define __USE_MISC	// clearenv
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

/*
 *   we are paraniod, so we _really_ clear the environment 
 */
void my_clearenv()
{
  char **p, *s;

  for (p = environ; *p; ++p) {
    s = *p;
    memset(s, 0, strlen(s));
  }
  clearenv();
}

/*
 *  appends string src to string dst 
 *  src string is copied until end-of-string or stop character (0x31) is encountered
 */
void my_strcat(char *dst, char *src)
{
  int len = strlen(dst); 
  dst += len;
  while (*src) {
    
    /* 0x31 is the stop character for our server */
    //if (*src == 0x31)
      //break;
    
    *dst = *src; 
    ++dst; ++src;
  }
}

/*
 *   print out string with a '--ECHO: ' prefix
 */
void echo(char *s)
{
  char buf[512] = "--ECHO: ";
	//printf("[Before Cat] Buffer dst is: %08lx, Ret address is: %0xlx\n", buf, (buf+4));
  my_strcat(buf, s);
	  // printf("[After Cat] Buffer dst is: %08lx, Ret address is: %0xlx\n", buf, (buf+4));
  fprintf(stdout, "%s\n", buf);
}

/* 
 *   simple echo service that prints out its first argument
 *   watch out for the stop character :-)
 */
int main(int argc, char **argv)
{

  if (argc != 2) {
    fprintf(stderr, "please provide one argument to echo\n");
    return 1;
  }
  my_clearenv();
  
  echo(argv[1]);

  return 0;
}
  
