/*
** server.c -- a stream socket server demo
** TCPServerDisplay
** Modification 1 (_M1): Can bind to a port # provided on the command line
** Modification 2 (_M2): Server receives a message and displays it
** Modification 3 (_M3): Display Datagram as individual bytes in hexadecimal
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// _M1 Not needed anymore  #define PORT "3490"	// the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

#define MAXDATASIZE 255 // _M2	max number of bytes we can get at once

void displayBuffer(char *Buffer, int length); // _M3
void performOperation(char op, char message[], int messageSize, char response[], int *responseSize);
void uppercase(char message[], int messageSize);
int isVowel(char letter);

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
}

return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// _M1 , now we need arguments int main(void)
int main(int argc, char *argv[]) // _M1 
{
	int sockfd, new_fd;	 // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;

	char s[INET6_ADDRSTRLEN];
	int rv;

	int numbytes; // _M2
	char buf[MAXDATASIZE]; // _M2


	/* _M1 Begin */
	if (argc != 2) {
		fprintf(stderr,"usage: TCPServerDisplay Port# \n");
		exit(1);
	}
	/* _M1 End*/

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, argv[1] /* _M1 PORT*/, &hints, &servinfo)) != 0) {
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

	if (p == NULL)	{
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

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

	while(1) 
	{	// main accept() loop
			sin_size = sizeof their_addr;
			new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
			if (new_fd == -1) {
				perror("accept");
				continue;
			}

		inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr),
		s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) 
		{ // this is the child process
			close(sockfd); // child doesn't need the listener
			/* _M2 Server will only receive */
			// _M2 if (send(new_fd, "Hello, world!", 13, 0) == -1)
			// _M2	perror("send");
			// _M2 Begin add
			if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) 
			{
				perror("recv");
				exit(1);
			}

			buf[numbytes] = '\0';

			printf("Server: received encoded message. Size: %d\n", buf[0]);
			// _M2 End add

			// displayBuffer(buf,numbytes); // _M3

			unsigned char totalMessageLength = buf[0];
			unsigned char requestID = buf[1];
			unsigned char operation = buf[2];

			char message[totalMessageLength - 3];
			//Get the message
			int i;
			for (i = 0; i < sizeof(message); i++) 
			{
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
			for (i = 0; i < responseSize; i++) 
			{
				buf[i + 2] = response[i];
			}

			//displayBuffer(buf,totalMessageLength); // _M3
			if (send(new_fd, buf, totalMessageLength, 0) == -1) 
			{
				perror("send");
				exit(1); 
			}

			printf("Sent response.\n");
			close(new_fd);
			exit(0);
		}
		
		close(new_fd);	// parent doesn't need this
	}

		return 0;
}

// _M3 Begin
void displayBuffer(char *Buffer, int length){
	int currentByte, column;

	currentByte = 0;
	printf("\n>>>>>>>>>>>> Content in hexadecimal <<<<<<<<<<<\n");
	while (currentByte < length){
		printf("%3d: ", currentByte);
		column =0;
		while ((currentByte < length) && (column < 10)){
			printf("%2x ",Buffer[currentByte]);
			column++;
			currentByte++;
		}
		printf("\n");
	}
	printf("\n\n");
}
// _M3 End


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
			for (j = 0; j < sizeof(consonants); j++) 
			{
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
		for (i = 0; i < *responseSize; i++) 
		{
			response[i] = message[i];
		}	
		break;

		case 80:
		for (i = 0; i < messageSize; i++)
			messageCopy[i] = message[i];
		uppercase(messageCopy, messageSize);

		for (i = 0; i < messageSize; i++) 
		{
			if (!isVowel(messageCopy[i])) 
			{
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

void uppercase(char message[], int messageSize) 
{
	int i;
	for (i = 0; i < messageSize; i++) 
	{
		message[i] = toupper(message[i]);
	}
}

int isVowel(char letter) 
{
	char vowels[6] = "AEIOUY";
	int i;
	for (i = 0; i < sizeof(vowels); i++) 
	{
		if (letter == vowels[i])
			return 1;
	}
	return 0;
}
