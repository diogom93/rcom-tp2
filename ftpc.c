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
#define C_PORT "21"

void check_URL(char *url, char *user, char *password, char *host, char *path);
char *build_command(char * cmd, char *arg);

int main(int argc, char *argv[]) 
{
	int sd, res, code, i;
	char buf[BUF_SIZE];
	char user[64], password[64], host[128], path[512], ip_str[INET6_ADDRSTRLEN], *cmd;
	struct addrinfo hints, *serv_info, *aux_p;

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
	
	/* Fill up hints struct to pass it to getaddrinfo() */
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; /* By using AF_UNSPEC we make the application independent of IP version */
	hints.ai_socktype = SOCK_STREAM; /* TPC stream sockets */
	hints.ai_flags = AI_PASSIVE; /* This tells getaddrinfo() to fill IP */
	
	res = getaddrinfo(host, C_PORT, &hints, &serv_info);
	if ( res != 0) {
		printf("Error! Couldn't get host information!\n");
		exit(1);
	}
	
	printf("List of IP adresses for %s:\n", host);
	for (aux_p = serv_info; aux_p != NULL; aux_p = aux_p->ai_next) {
		void *addr;
		char *ip_v;
		
		if (aux_p->ai_family == AF_INET) {
			struct sockaddr_in *ipv4 = (struct sockaddr_in *) aux_p->ai_addr;
			addr = &(ipv4->sin_addr);
			ip_v = "IPv4";
		} else {
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) aux_p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ip_v = "IPv6"; 
		}
		
		inet_ntop(aux_p->ai_family, addr, ip_str, sizeof(ip_str));
		printf("%s: %s\n", ip_v, ip_str);
	}
	
	sd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
	if (sd == -1) {
		printf("Error! Couldn't create socket!\n");
		exit(1);
	}
	
	res = connect(sd, serv_info->ai_addr, serv_info->ai_addrlen);
	if (res == -1) {
		printf("Error! Couldn't connect to host!\n");
	}
	
	/* Server response */
	
	recv(sd, buf, BUF_SIZE, 0);
	printf("\nR%s\n", buf);
	memset(buf, 0, BUF_SIZE);
	
	/* USER command */
	
	cmd = build_command("USER ", user);
	send(sd, cmd, strlen(cmd), 0);
	recv(sd, buf, BUF_SIZE, 0);
	printf("%s\n", buf);
	memset(buf, 0, BUF_SIZE);
	
	/* PASS command */
	
	cmd = build_command("PASS ", password);
	send(sd, cmd, strlen(cmd), 0);
	recv(sd, buf, BUF_SIZE, 0);
	printf("%s\n", buf);
	memset(buf, 0, BUF_SIZE);
	
	/* PASV command */
	
	send(sd, "PASV", strlen("PASV"), 0);
	recv(sd, buf, BUF_SIZE, 0);
	printf("%s\n", buf);
	memset(buf, 0, BUF_SIZE);
	
	close(sd);
	freeaddrinfo(serv_info);
	return 0;
}

/* Using this function the application can cover several cases of valid URL's */

void check_URL(char *url, char *user, char *password, char *host, char *path) {
	if (sscanf(url, "ftp://%[^:]:%[^@]@%[^/]/%s", user, password, host, path) == 4) {
		return;
	} else if (sscanf(url, "ftp://%[^:]:%[^@]@%[^/]", user, password, host) == 3) {
		strcpy(path, "Empty");
	} else if (sscanf(url, "ftp://%[^:@]:@%[^/]/%s", user, host, path) == 3) {
		return;
	} else if (sscanf(url, "ftp://%[^:@]@%[^/]/%s", user, host, path) == 3) {
		printf("Please enter email address: ");
		scanf("%s", password);
		return;
	} else if (sscanf(url, "ftp://%[^:@]:@%[^/]", user, host) == 2) {
		printf("Please enter email address: ");
		scanf("%s", password);
		strcpy(path, "Empty");
		return;
	} else if (sscanf(url, "ftp://%[^:@]@%[^/]", user, host) == 2) {
		printf("Please enter email address: ");
		scanf("%s", password);
		strcpy(path, "Empty");
		return;
	} else if (sscanf(url, "ftp://@%[^:@/]/%s", host, path) == 2) {
		printf("Please enter email address: ");
		scanf("%s", password);
		strcpy(user, "anonymous");
		return;
	} else if (sscanf(url, "ftp://%[^:@/]/%s", host, path) == 2) {
		printf("Please enter email address: ");
		scanf("%s", password);
		strcpy(user, "anonymous");
		return;
	} else if (sscanf(url, "ftp://@%[^:@/]/", host) == 1) {
		printf("Please enter email address: ");
		scanf("%s", password);
		strcpy(user, "anonymous");
		strcpy(path, "Empty");
		return;
	} else if (sscanf(url, "ftp://%[^:@/]/", host) == 1) {
		printf("Please enter email address: ");
		scanf("%s", password);
		strcpy(user, "anonymous");
		strcpy(path, "Empty");
		return;
	} else {
		printf("Error! URL should be in the following format: //<user>:<password>@<host>/<url-path>\n");
		exit(1);
	}
}

char * build_command (char * cmd, char * arg) {
	char * command;
	int cmd_len = sizeof(cmd) + sizeof(arg);
	command = malloc(cmd_len);
	
	strcpy(command, cmd);
	strcat(command, arg);
	strcat(command, "\n");
	
	return command;
}