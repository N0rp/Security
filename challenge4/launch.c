#include <stdio.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <sys/types.h>

#include <stdlib.h>
#include <errno.h>

int main(int argc, char ** argv)
{
	int sock;
	char envvar[100];
	
	if (argc < 2) {
		printf("USAGE: launch <program>\n");
		exit(1);
	}

	sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

	if (sock < 0)
	{
		printf("%s: opening socket failed with error %d\n", argv[0],errno);
		exit(1);
	}
	
	sprintf(envvar, "%d", sock);
	setenv("SOCK_FD", envvar, 1);
	printf("Executing...\n\n\n");
	execv(argv[1], &argv[1]);

	return 0;
}