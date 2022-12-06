#include "URL.h"

void newURL(URL *urlStruct) {
    memset(urlStruct->user, 0, sizeof(url_content));
    memset(urlStruct->password, 0, sizeof(url_content));
    memset(urlStruct->host, 0, sizeof(url_content));
    memset(urlStruct->path, 0, sizeof(url_content));
    memset(urlStruct->filename, 0, sizeof(url_content));
    urlStruct->port = 21;
}

// url = ftp://[<user>:<password>@]<host>/<url-path>
int parseURL(char *url, URL *urlStruct){

    char *expr;
    regex_t *regex;
    size_t nmatch = strlen(urlStr);
	regmatch_t pmatch[nmatch];
    char *element = (char*) malloc(strlen(url));
    char *tempUrl = (char*) malloc(strlen(url));
    int anonMode = 0;

    memcpy(tempUrl, url, strlen(url));

    if (strncmp(tempUrl, "ftp://", 6) != 0) {
        printf("Error: Not using ftp\n");
        return 1;
    }
    if (tempUrl[6] != '[') {
        anonMode = 1;
        expr = "ftp://([A-Za-z0-9.~-])+/([[A-Za-z0-9/~._-])+";
    }
    else{
        expr = "ftp://([([A-Za-z0-9])*:([A-Za-z0-9])*@])*([A-Za-z0-9.~-])+/([[A-Za-z0-9/~._-])+";
    }

    regex = (regex_t*) malloc(sizeof(regex_t));

    if(regcomp(regex, expr, REG_EXTENDED) != 0){
        printf("Error: Invalid URL.\n");
        return 1;
    }
    if(regexec(regex, tempUrl, nmatch, pmatch, REG_EXTENDED) != 0){
        printf("Error: URL couldnt execute.\n");
        return 1;
    }

    free(regex);

    //Remove ftp:// from string
    strcopy(tempUrl, tempUrl + 6);

    if (!anonMode) {
        //Remove [ from string
        strcopy(tempUrl, tempUrl + 1);

        //Get user
        strcpy(element, processElementUntilChar(tempUrl, ':'));
		memcpy(url->user, element, strlen(element));
        //Get password
        strcpy(element, processElementUntilChar(tempUrl, '@'));
		memcpy(url->password, element, strlen(element));
		strcpy(tempUrl, tempUrl + 1);
    }

    //Get host
    strcpy(element, processElementUntilChar(tempUrl, '/'));
	memcpy(url->host, element, strlen(element));

    //saving url path
	char* path = (char*) malloc(strlen(tempUrl));
	int startPath = 1;
	while (strchr(tempUrl, '/')) {
		element = processElementUntilChar(tempUrl, '/');

		if (startPath) {
			startPath = 0;
			strcpy(path, element);
		} else {
			strcat(path, element);
		}

		strcat(path, "/");
	}
	strcpy(url->path, path);

	// saving filename
	strcpy(url->filename, tempUrl);

	free(tempUrl);
	free(element);
}


int getIP(URL *urlStruct){
    struct hostent *host;
    if ((host = gethostbyname(urlStruct->host)) == NULL) {
        printf("Error: Could not get host by name.\n");
        return 1;
    }

    char *ip = inet_ntoa(*((struct in_addr *)host->h_addr));
    strcpy(urlStruct->ip, ip);
}

char* processElementUntilChar(char* str, char chr){
    char *temp = (char*) malloc(strlen(str));
    int i = strlen(str) - strlen(strcpy(temp, strchr(str, chr)));
    temp[i] = '\0';
    strncpy(temp, str, i);
    strcpy(str, str + len(temp) + 1);

    return temp;
}