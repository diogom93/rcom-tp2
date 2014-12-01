/* FTP Client - Computer Networks Assignment 2
 * Developed by:
 * Diogo Martins
 * Hugo Fonseca
 * Nuno Pires
 * 2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
#define C_PORT 21

void check_URL(char *url, char *user, char *password, char *host, char *path);

int main(int argc, char *argv[]) 
{
	int sfd, res;
	char buf[BUF_SIZE];
	char user[64], password[64], host[128], path[512];
	struct hostent *h;
	struct sockaddr_in channel;

	printf("FTP Client\n");
	printf("FEUP - Computer Networks - 2014/2015\n");
	printf("Developed by:\n* Diogo Martins\n* Hugo Fonseca\n* Nuno Pires\n");
	printf("Last compiled on %s at %s\n\n", __DATE__, __TIME__);
	
	if (argc < 2) {
			printf("Error! URL not specified!\n");
			exit(1);
		}
		
	check_URL(argv[1], user, password, host, path);	
	printf("User: %s\nPass: %s\nHost: %s\nPath: %s\n", user, password, host, path);
	
	h = gethostbyname(host);
	if (h == NULL) {
		printf("Couldn't find host!\n");
		exit(1);
	}
	
	/*		
	memset(&channel, 0, sizeof(channel));
		
	By using AF_UNSPEC we make the program independent of IP version
		
	channel.sin_family = AF_UNSPEC;
	sfd = socket(PF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
		
	if (sfd < 0) {
		printf("Couldn't create socket!\n");
		exit(1);
	}
		
	memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
	channel.sin_port = htons(C_PORT);
		
	res = connect(sfd, (struct sockaddr *)&channel, sizeof(channel));
		
	if (res < 0) {
		printf("Couldn't connect!\n");
		exit(1);
	}
		
	while (true) {
		res = read(sfd, buf, BUF_SIZE);
		if (res < 0) {
			printf("Error reading file!");
			exit(1);
		} else {
			write(STDOUT_FILENO, buf, res);
			break;
		}
	}*/
			
	return 0;
}

void check_URL(char *url, char *user, char *password, char *host, char *path) {
	if (sscanf(url, "ftp://%[^:]:%[^@]@%[^/]/%s", user, password, host, path) == 4) {
		return;
	} else if (sscanf(url, "ftp://%[^:]:%[^@]@%[^/]", user, password, host) == 3) {
		strcpy(path, "Empty");
	} else if (sscanf(url, "ftp://%[^:@]:@%[^/]/%s", user, host, path) == 3) {
		return;
	} else if (sscanf(url, "ftp://%[^:@]@%[^/]/%s", user, host, path) == 3) {
		strcpy(password, "Empty");
		return;
	} else if (sscanf(url, "ftp://%[^:@]:@%[^/]", user, host) == 2) {
		strcpy(password, "Empty");
		strcpy(path, "Empty");
		return;
	} else if (sscanf(url, "ftp://%[^:@]@%[^/]", user, host) == 2) {
		strcpy(password, "Empty");
		strcpy(path, "Empty");
		return;
	} else if (sscanf(url, "ftp://@%[^:@/]/%s", host, path) == 2) {
		strcpy(password, "Empty");
		strcpy(user, "anonymous");
		return;
	} else if (sscanf(url, "ftp://%[^:@/]/%s", host, path) == 2) {
		strcpy(password, "Empty");
		strcpy(user, "anonymous");
		return;
	} else if (sscanf(url, "ftp://@%[^:@/]/", host) == 1) {
		strcpy(password, "Empty");
		strcpy(user, "anonymous");
		strcpy(path, "Empty");
		return;
	} else if (sscanf(url, "ftp://%[^:@/]/", host) == 1) {
		strcpy(password, "Empty");
		strcpy(user, "anonymous");
		strcpy(path, "Empty");
		return;
	} else {
		printf("Error! URL should be in the following format: //<user>:<password>@<host>/<url-path>\n");
		exit(1);
	}
}