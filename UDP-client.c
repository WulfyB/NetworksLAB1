/*
** talker.c -- a datagram "client" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

//#define SERVERPORT "10023"	// the port users will be connecting to
#define MAXBUFLEN 252

static char requestID = 0;

typedef unsigned char uchar;

int main(int argc, char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int messageSize = strlen(argv[4]);
	struct packedMessage
	{
		uchar tMessageLength;
		uchar requestID;
		uchar operation;
		uchar *messageToSend = malloc(sizeof(uchar) * strlen(argv[4]));
	}__attribute__((__packed__));
		
	int rv;
	char buf[MAXBUFLEN];	
	int numbytes;

	if (argc != 5) {
		fprintf(stderr,"usage: talker hostname message\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	if(strcmp(argv[3], "5") != 0) 
	{
		fprintf(stderr, "Invalid Operation: Please input a valid operation.");
		return 1;
	}
	
	//char message[strlen(argv[4])] = argv[4];
	struct packedMessage pm = {(sizeof(*argv[4]) + 3), (uchar)requestID, *argv[3], *argv[4]};
	
	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}
					
	if ((numbytes = sendto(sockfd, (char*)&pm, sizeof(pm), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	
	//printf("%d\n", sizeof(pm));
	//printf("%d\n", strlen(argv[4]));

	freeaddrinfo(servinfo);

	printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
	
	unsigned int fromSize;
	struct sockaddr_storage fromAddr;
	if((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0,(struct sockaddr *)&fromAddr, &fromSize)) == 
-1)
	{
		perror("recvfrom");
		exit(1);
	}

	buf[numbytes] = '\0';
	printf("Echo: \"%s\"\n", buf);
	
	close(sockfd);

	return 0;

	if (requestID == 255)
		requestID = 0;
}
