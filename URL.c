#include "URL.h"

int getIP(char *host, URL *urlStruct){
    struct hostent *h;

    if ((h = gethostbyname(host)) == NULL){
        herror("gethostbyname()");
        return 1;
    }

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) h->h_addr)));

    strcpy(urlStruct->ip,inet_ntoa(*((struct in_addr *) h->h_addr)));

    return 0;
}

//ftp://[<user>:<password>@]<host>/<url-path>
int getFile(URL *urlStruct){
    if (strcmp(urlStruct->path, "") == 0){
        printf("Error: No path specified\n");
        return 1;
    }
    char path[256];
    strcpy(path, urlStruct->path);
    char* tk = strtok(path, "/");
    while(tk != NULL){
        strcpy(urlStruct->filename, tk);
        tk = strtok(NULL, "/");
    }
    return 0;
}

int parseData(char *url, URL *urlStruct){
    char* ftp = strtok(url, "/");       // ftp:
    char* urlrest = strtok(NULL, "/");  // <user
