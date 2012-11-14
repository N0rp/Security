/*
 **  Copyright notice: This file is copyright protected. iSecLab
 **  grants registered students the right to use and study
 **  the code. However, participants are not granted the right to distribute
 **  this code to third parties.
 **     
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* 
 *   A more advanced buffer overflow:
 *
 *   That pesky exit(0) prevents a standard overflow ;-)
 *
 *   The hint is: GOT is your friend.
 */

int main(int argc, char **argv)
{
  char *p;
  char buffer[128];

  if (argc != 3) {
    fprintf(stderr, "enter two strings that need to be concatenated\n");
    return 1;
  }

  /* concatenate two input strings, and print them out together */
  p = buffer + strlen(argv[1]);  

  strcpy(buffer, argv[1]);

  strcpy(p, argv[2]);

  fprintf(stdout, "%s\n", buffer);

  exit(0);
}
