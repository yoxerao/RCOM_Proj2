#include "FTP.h"

int startConnection(char *ip, int port, int *sockfd){
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }
    
    

    /*connect to the server*/
    if (connect(*sockfd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }

    return 0;

}


void getPortPlusIp(int *port, char *ip, FILE *socket){
    char* fullString = NULL;
    size_t numBytes;

    int foundLine = 0;
    while(!foundLine){
        getline(&fullString, &numBytes, socket);
        printf("Pasv mode reply   > %s", fullString);

        if (fullString[3] == ' ') {
            
            foundLine = 1;
        }
    }
    strtok(fullString, "(");

    // get the ip parts
    char *ip1 = strtok(NULL, ",");
    char *ip2 = strtok(NULL, ",");
    char *ip3 = strtok(NULL, ",");
    char *ip4 = strtok(NULL, ",");

    // get the port parts
    char *port1 = strtok(NULL, ",");
    char *port2 = strtok(NULL, ")");

    sprintf(ip, "%s.%s.%s.%s", ip1, ip2, ip3, ip4); // join the ip parts together
    *port = atoi(port1)*256 + atoi(port2); // calculate the port number 

}


//sends a command over a socket connection
int sendCommand(int socket, char * cmd){
    int bytesSent = write(socket, cmd, strlen(cmd));

    if (bytesSent < 0){
        perror("Error sending command");
        return 1;
    }
    
    return 0;
}

int readReply(FILE * socket){
    char * reply = NULL;
    size_t reply_size = 0;

    while(getline(&reply, &reply_size, socket) > 0){
        // I have absolutely no clue why, but even though the server always responds with a 3 digit code, 
        // we get a segmentation fault before sending crefentials to the server if we don't check for a space after the 3rd character
        if(reply[3] == ' '){
            if (reply[0] == '1'){
            printf("Information reply > %s", reply);
            }
            else if (reply[0] == '2'){
                printf("Success reply     > %s", reply);
            }
            else if (reply[0] == '3'){
                printf("Redirect reply    > %s", reply);
            }
            else if (reply[0] == '4'){
                printf("Temporary error   > %s", reply);
                exit(1);
            }
            else if (reply[0] == '5'){
                printf("Permanent error   > %s", reply);
                exit(1);
            }
            else {
                printf("Unknown reply     > %s", reply);
                return -1;
            }
            break;
        }
        if (reply[0] == '1'){
            printf("Information reply > %s", reply);
        }
        else if (reply[0] == '2'){
            printf("Success reply     > %s", reply);
        }
        else if (reply[0] == '3'){
            printf("Redirect reply    > %s", reply);
        }
        else if (reply[0] == '4'){
            printf("Temporary error   > %s", reply);
            exit(1);
        }
        else if (reply[0] == '5'){
            printf("Permanent error   > %s", reply);
            exit(1);
        }
        else {
            printf("Unknown reply     > %s", reply);
            return -1;
        }
 
    }

    free(reply);
    return 0;
}

int writeToFile(char *fileN, int socket) {
 
  FILE *file = fopen(fileN, "w");
  if (file == NULL) {
    printf("Error opening file\n");
    return 1;
  }

  
  char *buffer = malloc(BUFSIZ);
  if (buffer == NULL) {
    printf("Error allocating memory\n");
    return 1;
  }

  // Read data from the socket and write it to the file
  ssize_t bytesRead;
  while ((bytesRead = recv(socket, buffer, BUFSIZ, 0)) > 0) {
    fwrite(buffer, 1, bytesRead, file);
  }


  if (bytesRead < 0) {
    perror("Error reading from socket");
    return 1;
  }

  
  free(buffer);
  fclose(file);
  return 0;
}
