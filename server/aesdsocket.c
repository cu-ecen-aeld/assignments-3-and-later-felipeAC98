#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){

	int sockfd = socket(PF_INET, SOCK_STREAM, 0); //returns a file descriptor
	
	if(sockfd==-1){
		perror("socket");
		return -1;
	}

	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;

	//getting the sock address to use inside bind
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL,"9000", &hints, &servinfo)) !=0){
		syslog(LOG_USER, "Failure while getting getaddinfo");
		return -1;
	}

	//Binding the port
	int _bind = bind(sockfd, servinfo->ai_addr, sizeof(struct sockaddr));
	
	if(_bind==-1){
		perror("bind");
		close(sockfd);
		return -1;
	}

	//listning to the socket
	int backlog=5; //number of pending connections allowed before refusing
	if(listen(sockfd, backlog)==-1){
		perror("listen");
		return -1;
	}

	//accepting
	struct sockaddr acc_sockaddr;
	socklen_t len_acc_sockaddr = sizeof(acc_sockaddr);
	int acceptedfd = accept(sockfd, &acc_sockaddr, &len_acc_sockaddr);

	if(acceptedfd == -1){
		perror("accept");
		return -1;
	}

	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(((struct sockaddr_in *)&acc_sockaddr)->sin_addr), client_ip, INET_ADDRSTRLEN);

	syslog(LOG_USER,"Accepted connection from %s",client_ip);
	printf("Accepted connection from %s\n",client_ip);

	/*int stop_received=0;
	while(stop_received==0){

        	file = open("/var/tmp/aesdsocketdata", O_RDWR | O_CREAT | O_TRUNC, 0755);

        	if (write(file, text, strlen(text))==-1){
               		close(file);
                	return 1;
        	}

        	close(file);
	}//*/

	
	freeaddrinfo(servinfo);

        return 0;
}

