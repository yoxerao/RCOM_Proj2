#pragma once
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

int startConnection(char *ip, int port, int *sockfd);
void getPortPlusIp(int *port, char *ip, FILE *socket);
int sendCommand(int socket, char * cmd);
int readReply(FILE * socket);
int writeToFile(char *file, int socket );

