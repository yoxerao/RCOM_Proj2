#pragma once
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
    char *scheme;
    char *user;
    char *password;
    char *host;
    char *path;
    char *filename;
    char *ip;
    int port;
} URL;


int getIP(char *host, URL *urlStruct);
int parseURL(char *url, URL *urlStruct);