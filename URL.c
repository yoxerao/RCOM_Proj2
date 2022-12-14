#include "URL.h"

int getIP(char *host, URL *urlStruct) {
    struct hostent *h;

    if ((h = gethostbyname(host)) == NULL) {
        herror("gethostbyname()");
        return 1;
    }

    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) h->h_addr)));


    int ipLen = strlen(inet_ntoa(*((struct in_addr *) h->h_addr)));
    urlStruct->ip = malloc(ipLen + 1);


    strcpy(urlStruct->ip, inet_ntoa(*((struct in_addr *) h->h_addr)));

  return 0;
}

int parseURL(char *url, URL *urlStruct){
    // extract scheme, needs to be "ftp"
    char *schemePtr = strstr(url, "://");
    int schemeLen = schemePtr - url;
    
    urlStruct->scheme = malloc(schemeLen + 1);
    strncpy(urlStruct->scheme, url, schemeLen);
    urlStruct->scheme[schemeLen] = '\0';
    if (strcmp(urlStruct->scheme, "ftp") != 0){
        printf("Wrong protocol. Only ftp is supported.\n");
        return 1;
    }
    printf("Scheme: %s\n", urlStruct->scheme);

    // Check if the URL has a user and password
    char *userPassPtr = strstr(schemePtr + 3, "@");
    if (userPassPtr) {
        // Extract the user + password
        char *userPtr = schemePtr + 3;
        char *passwordPtr = strstr(userPtr, ":");
        int userLen = passwordPtr - userPtr;
        urlStruct->user = malloc(userLen + 1);
        strncpy(urlStruct->user, userPtr, userLen);
        urlStruct->user[userLen] = '\0';
        printf("user: %s\n", urlStruct->user);

        passwordPtr++; // skip ':' 
        int passwordLen = userPassPtr - passwordPtr;
        urlStruct->password = malloc(passwordLen + 1);
        strncpy(urlStruct->password, passwordPtr, passwordLen);
        urlStruct->password[passwordLen] = '\0';
        printf("Password: %s\n", urlStruct->password);
    } else {
        // "anonymous" and "pass" when no user and pass in url
        urlStruct->user = strdup("anonymous");
        urlStruct->password = strdup("pass");
        printf("user: anonymous\n");
        printf("Password: pass\n");
    }

    // Extract host + path
    char *hostPtr = userPassPtr ? userPassPtr + 1 : schemePtr + 3;
    char *pathPtr = strstr(hostPtr, "/");
    int hostLen = pathPtr - hostPtr;
    urlStruct->host = malloc(hostLen + 1);
    strncpy(urlStruct->host, hostPtr, hostLen);
    urlStruct->host[hostLen] = '\0';

    printf("Host: %s\n", urlStruct->host);

    pathPtr++; // skip '/'
    int pathLen = strlen(pathPtr);
    urlStruct->path = malloc(pathLen + 1);
    strncpy(urlStruct->path, pathPtr, pathLen);
    urlStruct->path[pathLen] = '\0';

    printf("Path: %s\n", urlStruct->path);

    // Extract the filename
    char *filenamePtr = strrchr(pathPtr, '/');
    
    filenamePtr++; // skip '/'
    int filenameLen = strlen(filenamePtr);
    urlStruct->filename = malloc(filenameLen + 1);
    strncpy(urlStruct->filename, filenamePtr, filenameLen);
    urlStruct->filename[filenameLen] = '\0';

    printf("Filename: %s\n", urlStruct->filename);


    if(getIP(urlStruct->host,urlStruct) != 0){
        printf("Error getting host name\n");
        return 1;
    }

    return 0;
}