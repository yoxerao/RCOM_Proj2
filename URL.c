#include "URL.h"

int getIP(char *host, URL *urlStruct){
    struct hostent *h;

    if ((h = gethostbyname(host)) == NULL){
        herror("gethostbyname()");
        return 1;
    }

    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) h->h_addr)));

    strcpy(urlStruct->ip,inet_ntoa(*((struct in_addr *) h->h_addr)));

    return 0;
}

int parseURL(char *url, URL *urlStruct){
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

    if(getIP(urlStruct->host,urlStruct) != 0){
        printf("Error getting host name\n");
        return 1;
    }

    // extract the file name from the path
    strcpy(fullpath, urlStruct->path);
    char* token2 = strtok(fullpath, "/");
    while( token2 != NULL ) {
        strcpy(urlStruct->filename, token2);
        token2 = strtok(NULL, "/");
    }

    return 0;
}