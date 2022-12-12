#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#include <string.h>

// URL structure
typedef struct URL{
    char user[256];
    char password [256];
    char host[256];
    char path[256];
    char filename[256];
    char ip[256];
    int port;
} URL;

int getIp(char *host, struct URL *urlStruct){
    struct hostent *h;

    if ((h = gethostbyname(host)) == NULL){
        herror("gethostbyname()");
        return 1;
    }

    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) h->h_addr)));

    strcpy(urlStruct->ip,inet_ntoa(*((struct in_addr *) h->h_addr)));

    return 0;
}

int parseUrl(char *url, struct URL *urlStruct ){
    char fullpath[256];
    char* token;
    char* ftp = strtok(url, "/");       // ftp:
    char* urlrest = strtok(NULL, "/");  // [<user>:<password>@]<host>
    char* path = strtok(NULL, "");      // <url-path>

    if (strcmp(ftp, "ftp:") != 0){
        printf("Error: Not using ftp\n");
        return 1;
    }

    char* user = strtok(urlrest, ":");
    char* pass = strtok(NULL, "@");

    // no user:password given
    if (pass == NULL)
    {
        user = "anonymous";
        pass = "pass";
        strcpy(urlStruct->host, urlrest);
    } else
        strcpy(urlStruct->host, strtok(NULL, ""));

    strcpy(urlStruct->path, path);
    strcpy(urlStruct->user, user);
    strcpy(urlStruct->password, pass);

    if(getIp(urlStruct->host,urlStruct) != 0){
        printf("Error resolving host name\n");
        return 1;
    }

    // extract the file name from the path
    strcpy(fullpath, urlStruct->path);
    token = strtok(fullpath, "/");
    while( token != NULL ) {
        strcpy(urlStruct->filename, token);
        token = strtok(NULL, "/");
    }

    return 0;
}


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
        printf("> %s", fullString);

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
    long code;
    char * end;

    while(getline(&reply, &reply_size, socket) > 0){

        if(reply[3] == ' '){
            code = strtol(reply, &end, 10);
            if(code >= 500 && code <= 559){
                printf("Error code: %ld\n", code);
                exit(1);
            }
            break;
        }
    }

    free(reply);
    return 0;
}

int writeToFile(char *file, int socket ){
    FILE *file = fopen(file, "w");

    if(file == NULL){
        printf("Error opening file\n");
        return 1;
    }

    char *c = malloc(1);
    if (c == NULL)
    {
        printf("Error allocating memory\n");
        return 1;
    }
    
    while(read(socket, c, 1) > 0){  // read one byte at a time
        fputc(*c, file);
    }

    free(c);
    fclose(file);
    return 0;
}



int main(int argc, char **argv) {

    if(argc != 2){
          printf("Incorrect program usage\n"
               "Usage: download ftp://<user>:<password>@<host>/<url> \n"
               "Example anonymous user: download ftp://ftp.up.pt/pub/kodi/timestamp.txt \n"
               "Example other user: download ftp://userName:password@ftp.up.pt/pub/kodi/timestamp.txt \n");
        exit(1);
    }

    struct data dataInfo;
    int sockfd, sockfdReceive;

    if(parseData(argv[1], &dataInfo) != 0){
        printf("Error parsing input\n");
        return 1;
    }
    
    if(startConnection(dataInfo.ip, 21, &sockfd) != 0){
        printf("Error starting connection\n");
        return 1;
    }

    FILE * readSockect = fdopen(sockfd, "r");
    
    readReply(readSockect);
    
    //credentials
    char command[256];
    sprintf(command, "user %s\n",dataInfo.user);
    sendCommand(sockfd,command);
    //sleep(1);
    readReply(readSockect);
    sprintf(command, "pass %s\n",dataInfo.password);
    sendCommand(sockfd,command);
    readReply(readSockect);
    
    //set mode
    sprintf(command, "pasv \n");
    sendCommand(sockfd,command);

    //read ip and port 
    char ip[32];
    int port;
    readIpPort(ip, &port, readSockect);
    printf("ip: %s\n",ip);
    printf("port: %i\n", port);

    //open connection to receive data
    if(startConnection(ip, port, &sockfdReceive) != 0){
        printf("Error starting connection\n");
        return 1;
    }

    sprintf(command, "retr %s\r\n",dataInfo.path);
    sendCommand(sockfd,command);
    readReply(readSockect);

    readToFile(dataInfo.fileName, sockfdReceive);

    //close
    sprintf(command, "quit \r\n");
    sendCommand(sockfd,command);
    
    return 0;

}