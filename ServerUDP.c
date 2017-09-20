/*
** ServerUDP.c -- UDP Server for Lab 1
*  20 September 2017 - Process string before returning to client -DH
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

#define MYPORT "10023"	// the port users will be connecting to

#define MAXBUFLEN 100

void performOperation(char op, char message[], int messageSize, char response[], int *responseSize);
void uppercase(char message[], int messageSize);
int isVowel(char letter);


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);

	printf("listener: waiting to recvfrom...\n");

	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}

	printf("listener: got packet from %s\n",
		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s));
	printf("listener: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';
	printf("listener: packet contains \"%s\"\n", buf);
  //process string here before returning
  
  unsigned char totalMessageLength = buf[0];
  unsigned char requestID = buf[1];
  unsigned char operation = buf[2];
  char message[totalMessageLength - 3];
  
  //Get the message
	int i;
	for (i = 0; i < sizeof(message); i++) {
		 message[i] = buf[i + 3];
	}
  printf("size of message: %d\n", sizeof(message)); 
  
  //process request here
	char response[252];
	int responseSize = 0;

	performOperation(operation, message, sizeof(message), response, &responseSize);	  
	totalMessageLength = responseSize + 2;
	printf("total message length: %d\n", totalMessageLength);
		 
	buf[0] = totalMessageLength;
	buf[1] = requestID;
	printf("response size: %d\n", responseSize); 
		 
  //Add response back to buffer
	for (i = 0; i < responseSize; i++) {
	buf[i + 2] = response[i];
  }
  
  //end processing string
	if(sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&their_addr, sizeof(their_addr)) != 
  strlen(buf))
	{
		perror("Failed to echo");
	}
	
	close(sockfd);

	return 0;
}
//Thanks to Lane for the following:
void performOperation(char op, char message[], int messageSize, char response[], int *responseSize){
	int i = 0;
	int j = 0;
	char consonants[20] = "BCDFGHJKLMNPQRSTVXZW";
	char consonantCount = 0;
	char messageCopy[messageSize];
	switch(op) {

		case 5:
		uppercase(message, messageSize);
		for (i = 0; i < messageSize; i++) {
			for (j = 0; j < sizeof(consonants); j++) {
				if (message[i] == consonants[j])
					consonantCount++;		
			}
		}
		response[0] = consonantCount;
		*responseSize = 1;
		break;

		case 10 : 
		uppercase(message, messageSize);
		*responseSize = messageSize;
		for (i = 0; i < *responseSize; i++) {
			response[i] = message[i];
		}	
		break;

		case 80:
		for (i = 0; i < messageSize; i++)
			messageCopy[i] = message[i];
		uppercase(messageCopy, messageSize);
		
		for (i = 0; i < messageSize; i++) {
			if (!isVowel(messageCopy[i])) {
				response[j] = message[i];
				j++;
			}
		}
		*responseSize = j;
		break;
	default:
	break;
	}

}

void uppercase(char message[], int messageSize) {
	int i;
	for (i = 0; i < messageSize; i++) {
		message[i] = toupper(message[i]);
	}
}

int isVowel(char letter) {
	char vowels[6] = "AEIOUY";
	int i;
	for (i = 0; i < sizeof(vowels); i++) {
		if (letter == vowels[i])
			return 1;
	}
	return 0;
}
