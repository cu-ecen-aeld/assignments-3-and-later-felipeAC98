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

#define BUFFER_SIZE 2

int read_socket(int);
int send_socket(int);

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
	
	if(read_socket(acceptedfd)==-1){
		return -1;
	}


	if(send_socket(acceptedfd) == -1)
		return -1;

	freeaddrinfo(servinfo);

        return 0;
}

int send_socket(int acceptedfd){
	int BUFFER_SIZE2=16;
	int fd;
        char text[BUFFER_SIZE2];
	ssize_t bytes_read;

	fd = open("/var/tmp/aesdsocketdata", O_RDONLY);
	if(fd==-1){
		perror("open");
		return -1;	
	}

	bytes_read=read(fd, text, BUFFER_SIZE2-1);
	printf("bytes_read: %d\n",bytes_read);
	while(bytes_read>0){
		printf("sending: %s\n",text);
		

		if(send(acceptedfd,text,bytes_read,0) == -1){
			perror("send");
			return -1;
		}
		bytes_read=read(fd, text, BUFFER_SIZE2-1);
		printf("bytes_read: %d\n",bytes_read);
	}

	close(fd);

	return 0;
}

int read_socket(int acceptedfd){
        int stop_received=0;
        char text[BUFFER_SIZE];
        int file = open("/var/tmp/aesdsocketdata", O_RDWR | O_CREAT | O_TRUNC, 0755);

        while(stop_received==0){

                //Receive the data from socket
                int bytes_received=recv(acceptedfd, text, BUFFER_SIZE-1,0);

                if(bytes_received == -1){
                        perror("read");
			close(file);
                        return -1;
                }

                if(bytes_received==0){
                        //text[0]='\n';
                        stop_received=1;
                }
                else{
			text[bytes_received]='\0';
                        printf("value read: %s\n",text);
			
			//writing the data into file
            		if (write(file, text, 1)==-1){
              	         	close(file);
                	        perror("write");
                        	return -1;
                	}
		}

        }//*/
        close(file);

	return 0;
}
