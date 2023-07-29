#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 16

int sockfd;
int acceptedfd;

int read_socket(int);
int send_socket(int);
void signal_handler(int);

int main(int argc, char *argv[]){

	remove("/var/tmp/aesdsocketdata");
	int daemon_mode=0;

	
	if(getopt(argc, argv, "d")=='d'){
	//if (strcmp(argv[1], "-d") == 0){
		daemon_mode=1;
		printf("daemon mode enabled\n");	
	}

	sockfd=socket(PF_INET, SOCK_STREAM, 0); //returns a file descriptor
		
	if(sockfd==-1){
		perror("socket");
		return -1;
	}

	//enable SO_REUSEADDR
	int optval=1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        	perror("setsockopt");
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

	freeaddrinfo(servinfo);

	if(daemon_mode==1){
		pid_t pid = fork();

    		if (pid < 0) {
       		perror("fork");
        	return -1;
    		}

    		// If we get a valid PID, this is the parent process, so we can exit.
    		if (pid > 0) {
        		exit(EXIT_SUCCESS);
    		}
	}

	if (signal(SIGINT, signal_handler) == SIG_ERR) {
        	perror("Error setting SIGINT handler");
        	return EXIT_FAILURE;
    	}

    	if (signal(SIGTERM, signal_handler) == SIG_ERR) {
        	perror("Error setting SIGTERM handler");
        	return EXIT_FAILURE;
	}
	while(1){
		//listning to the socket
		int backlog=1; //number of pending connections allowed before refusing
		if(listen(sockfd, backlog)==-1){
			perror("listen");
			return -1;
		}

		//accepting
		struct sockaddr acc_sockaddr;
		socklen_t len_acc_sockaddr = sizeof(acc_sockaddr);
		acceptedfd = accept(sockfd, &acc_sockaddr, &len_acc_sockaddr);

		if(acceptedfd == -1){
			perror("accept");
			return -1;
		}

		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(((struct sockaddr_in *)&acc_sockaddr)->sin_addr), client_ip, INET_ADDRSTRLEN);

		syslog(LOG_INFO,"Accepted connection from %s",client_ip);

		if(read_socket(acceptedfd)==-1){
			perror("read");
			close(acceptedfd);
			close(sockfd);
			return -1;
		}


		if(send_socket(acceptedfd) == -1){
	        	perror("send");
                        close(acceptedfd);
                        close(sockfd);

			return -1;
		}
		syslog(LOG_INFO, "Closed connection from %s", client_ip);
		close(acceptedfd);
	}
	close(sockfd);

        return 0;
}

int send_socket(int acceptedfd){
	int BUFFER_SIZE2=BUFFER_SIZE;
	int fd;
        char text[BUFFER_SIZE2];
	ssize_t bytes_read;

	fd = open("/var/tmp/aesdsocketdata", O_RDONLY);
	if(fd==-1){
		perror("open");
		return -1;	
	}

	bytes_read=read(fd, text, BUFFER_SIZE2);
	ssize_t bytes_written;
	while(bytes_read>0){
		//printf("bytes_read: %d\n",bytes_read);
		printf("sending: %s\n",text);
		bytes_written=send(acceptedfd,text,bytes_read,0);
		//printf("bytes_written: %d\n",bytes_written);

		if(bytes_written == -1){
			perror("send");
			return -1;
		}
		bytes_read=read(fd, text, BUFFER_SIZE2);
	}

	close(fd);
	return 0;
}

int read_socket(int acceptedfd){
    FILE *file;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Open the file in append mode for writing
    file = fopen("/var/tmp/aesdsocketdata", "a");
    if (file == NULL) {
        syslog(LOG_ERR, "Error opening file");
        return -1;
    }

    // Receive data from the client and append it to the file
    while ((bytes_received = recv(acceptedfd, buffer, sizeof(buffer), 0)) > 0) {
        // Find the first newline character in the received data
        char *newline_ptr = memchr(buffer, '\n', bytes_received);
        printf("buffer write:%s\n",buffer);
        if (newline_ptr != NULL) {
            // Calculate the size of the data before the newline
            size_t data_size = newline_ptr - buffer + 1;

            // Append the data to the file
            fwrite(buffer, 1, data_size, file);

            // Stop receiving more data after the newline
            break;
        } else {
            // Append the entire buffer to the file
            fwrite(buffer, 1, bytes_received, file);
        }
    }

    // Close the file and the client socket
    fclose(file);
	
    return 0;
}

void signal_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nReceived SIGINT (Ctrl+C).\n");
    } else if (signum == SIGTERM) {
        printf("\nReceived SIGTERM.\n");
    }

    close(sockfd);
    close(acceptedfd);
    remove("/var/tmp/aesdsocketdata");
    syslog(LOG_INFO,"Caught signal, exiting");


    exit(EXIT_SUCCESS);
}
