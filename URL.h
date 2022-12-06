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

typedef char url_content[256];

typedef struct URL{
    char *user;
    char *password;
    char *host;
    char *ip;
    char *path;
    char *filename;
    int port;
} URL;


int parseURL(char *url, URL *urlStruct);
void initURL(URL *urlStruct);
int getIP(URL *urlStruct);

char* processElementUntilChar(char* str, char chr);
