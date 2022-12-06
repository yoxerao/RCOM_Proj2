#pragma once
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


//ftp://[<user>:<password>@]<host>/<url-path>
typedef struct URL{
    char user[256];
    char password [256];
    char host[256];
    char path[256];
    char filename[256];
    char ip[256];
    int port;
} URL;


int getIP(char *host, URL *urlStruct);
int getFile(URL *urlStruct);
int parseURL(char *url, URL *urlStruct);