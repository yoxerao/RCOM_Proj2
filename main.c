#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include "URL.h"
#include "FTP.h"

int main(int argc, char **argv) {

    if(argc != 2){
        exit(1);
    }

    struct URL urlStruct;
    int socketfd, socket;

	// Parse the URL and check for errors
    if(parseURL(argv[1], &urlStruct) != 0){
        printf("Error parsing input\n");
        return 1;
    }
    
	// Connect to the FTP server and check for errors
    if(startConnection(urlStruct.ip, 21, &socketfd) != 0){ // check port config
        printf("Error starting connection\n");
        return 1;
    }

	// Open the socket for reading and writing
    FILE * readSocket = fdopen(socketfd, "r");
	// Read the initial reply from the FTP server
    if (readReply(readSocket) != 0){
		return 1;
	}
    
    
    char cmd[256];
	// Log in to the FTP server using the provided username and password
    sprintf(cmd, "user %s\n",urlStruct.user);
    if (sendCommand(socketfd,cmd) != 0){
		return 1;
	}
    if (readReply(readSocket) != 0){
		return 1;
	}

    sprintf(cmd, "pass %s\n",urlStruct.password);
    if (sendCommand(socketfd,cmd) != 0){
		return 1;
	}
	if (readReply(readSocket) != 0){
		return 1;
	}
    
    // Enter passive mode and retrieve the server's IP and port
    sprintf(cmd, "pasv \n");
    if (sendCommand(socketfd,cmd) != 0){
		return 1;
	}

    //read ip and port 
    char ip[32];
    int port;
    if (getPortPlusIp(ip, &port, readSocket) != 0){
		printf("Error getting port and ip\n");
		return 1;
	}

    // Connect to the server on the provided IP and port and retrieve the file
    if(startConnection(ip, port, &socket) != 0){
        printf("Error starting connection\n");
        return 1;
    }

    sprintf(cmd, "retr %s\r\n",urlStruct.path); // retrieve file from path
    if(sendCommand(socketfd,cmd) != 0){
		return 1;
	}
    if(readReply(readSocket) != 0){
		return 1;
	}
	// Write the contents of the file to a local file with the same name
    if(writeToFile(urlStruct.filename, socket) != 0){
		return 1;
	}

    // Quit the FTP session and close the connection
    sprintf(cmd, "quit \r\n");
    if(sendCommand(socketfd,cmd) != 0){
		return 1;
	}
    
    return 0;

}