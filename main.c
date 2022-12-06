#include <stdio.h>
#include "URL.h"
#include "FTP.h"


int main(int argc, char** argv) {
	if(argc != 2){
		printf("Wrong number of arguments.\n");
		return -1;
	}

	printf("-------------------- URL PARSER --------------------\n\n");
	URL url;
	newURL(&url);

	// start parsing argv[1] to URL components
	if (parseURL(&url, argv[1]))
		return -1;

	// edit url ip by hostname
	if (getIpByHost(&url)) {
		printf("ERROR: Cannot find ip to hostname %s.\n", url.host);
		return -1;
	}
	printf("URL parsed successfully!\n");
	printf("Host: %s\n", url.host);
	printf("IP: %s\n\n", url.ip);

	printf("-------------------- FTP CLIENT --------------------\n\n");

	FTP ftp;
	ftpConnect(&ftp, url.ip, url.port);

	char* user;
	if (strlen(url.user)) {
		user = url.user;
	} else {
		user = "anonymous";
	}
	
	
	char* password;
	if (strlen(url.password)) {
		password = url.password;
	} else {
		password = "pass";
	}

	// Sending credentials to server
	if (ftpLogin(&ftp, user, password)) {
		printf("ERROR: Cannot login user %s\n", user);
		return -1;
	}

	// Changing directory
	if (ftpCWD(&ftp, url.path)) {
		printf("ERROR: Cannot change directory to the folder of %s\n",
				url.filename);
		return -1;
	}

	// Entry in passive mode
	if (ftpPasv(&ftp)) {
		printf("ERROR: Cannot entry in passive mode\n");
		return -1;
	}

	// Begins transmission of a file from the remote host
	ftpRetr(&ftp, url.filename);

	// Starting file transfer
	ftpDownload(&ftp, url.filename);

	// Disconnecting from server
	ftpDisconnect(&ftp);

	return 0;
}

void printUsage(char* argv0) {
	printf("\nUsage1 Normal: %s ftp://[<user>:<password>@]<host>/<url-path>\n",
			argv0);
	printf("Usage2 Anonymous: %s ftp://<host>/<url-path>\n\n", argv0);
}
