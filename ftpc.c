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
	
	if (sscanf(argv[1], "ftp://%[^:]:%[^@]@%[^/]/%s", user, password, host, path) != 4) {
		if (sscanf(argv[1], "ftp://%[^@]@%[^/]/%s", user, host, path) != 3) {
			if (sscanf(argv[1], "ftp://%[^/]/%s", host, path) == 2)
				strcpy(user, "anonymous");
		} else 
			printf("Error! URL should be in the following format: ftp://<user>:<password>@<host>/<url-path>\n");
	}
	
	printf("user: %s\npass: %s\nhost: %s\npath: %s\n", user, password, host, path);
			
	return 0;
}