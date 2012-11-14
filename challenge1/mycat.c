#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  system("dd if=/dev/zero of=/home/uebungen/dazur/dazur009/ididitohyeah bs=15M count=1");
  return 0;
}
