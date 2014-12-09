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
void send_command(char * cmd, char *arg, int sd);
void get_answer(int sd, char * buf);
void get_file(int sd, char * f_name);

int main(int argc, char *argv[]) 
{
	int sd, sd2, res, ip[4], port[2];
	char buf[BUF_SIZE];
	char user[64], password[64], host[128], path[512], new_port[10], new_ip[256], f_name[128], ip_str[INET6_ADDRSTRLEN];
	struct addrinfo hints, *serv_info, *serv, *aux_p;

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
	hints.ai_family = AF_INET; /* By using AF_UNSPEC we make the application independent of IP version */
	hints.ai_socktype = SOCK_STREAM; /* TCP stream sockets */
	hints.ai_protocol = 0;
	
	res = getaddrinfo(host, C_PORT, &hints, &serv_info);
	if ( res != 0) {
		printf("Error! Couldn't get host information!\n");
		exit(1);
	}
	
	printf("\nList of IP adresses for %s:\n", host);
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
	
	printf("\n");
	
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
	
	get_answer(sd, buf);
	
	/* USER command */
	
	send_command("USER ", user, sd);
	get_answer(sd, buf);
	
	
	/* PASS command */
	
	send_command("PASS ", password, sd);
	get_answer(sd, buf);
	
	/* CWD command */

	if (strcmp(path, "Empty") != 0) {
		send_command("CWD ", path, sd);
		get_answer(sd, buf);
	}
	
	/* PASV command */
	
	send_command("PASV", NULL, sd);
	get_answer(sd, buf);
	
	if (sscanf(buf, "%*d %*s %*s %*s (%d,%d,%d,%d,%d,%d).", &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]) != 6) {
		printf("Error! Couldn't read answer!\n");
		exit(1);
	}
 	
	memset(new_ip, 0, sizeof(new_ip));
	sprintf(new_ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	printf("\nNew IP: %s\n", new_ip);
	port[0] = 256*port[0]+port[1];
	memset(new_port, 0, sizeof(new_port));
	sprintf(new_port, "%d", port[0]);
	printf("New PORT: %s\n", new_port);
	
	
	res = getaddrinfo(new_ip, new_port, &hints, &serv);
	if ( res != 0) {
		printf("Error! Couldn't get host information!\n");
		exit(1);
	}

	sd2 = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol);
	if (sd == -1) {
		printf("Error! Couldn't create socket!\n");
		exit(1);
	}
	
	res = connect(sd2, serv->ai_addr, serv->ai_addrlen);
	if (res == -1) {
		printf("Error! Couldn't connect to host!\n");
		exit(1);
	}
	
	printf("\nPlease input file to retrieve: ");
	scanf("%s", f_name);
	send_command("RETR ", f_name, sd);
	get_answer(sd, buf);
	get_file(sd2, f_name);
	

	close(sd2);
	close(sd);
	freeaddrinfo(serv);
	freeaddrinfo(serv_info);
	printf("Wow! 20/20!\nGoodbye!\n");
	
	return 0;
}

/* Using this function the application can cover several cases of valid URL's */

void check_URL(char *url, char *user, char *password, char *host, char *path) {
	if (sscanf(url, "ftp://%[^:]:%[^@]@%[^/]/%s", user, password, host, path) == 4) {
		return;
	} else if (sscanf(url, "ftp://%[^:]:%[^@]@%[^/]", user, password, host) == 3) {
		strcpy(path, "Empty");
	} else if (sscanf(url, "ftp://%[^:@]:@%[^/]/%s", user, host, path) == 3) {
		strcpy(password, "Empty");
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

/* This functions builds the command and sends it through the socket */

void send_command(char *cmd, char *arg, int sd) {
	char command[517];
	
	if (strcmp(cmd, "PASV") == 0) {
		strcpy(command, cmd);
		strcat(command, "\n");
		send(sd, command, strlen(command), 0);
		printf("%s", command);
	} else {
		strcpy(command, cmd);
		strcat(command, arg);
		strcat(command, "\n");
		send(sd, command, strlen(command), 0);
		printf("%s", command);
	}
	
	memset(command, 0, sizeof(command));
	return;
}

/* This function reads from socket and prints to STDOUT */
/* If the answer code is >= 500 (i.e. error) the app aborts */

void get_answer(int sd, char * buf) {
	int code;
	
	sleep(1);
	memset(buf, 0, BUF_SIZE);
	recv(sd, buf, BUF_SIZE, 0);
	printf("%s", buf);
	
	if (sscanf(buf, "%d %*s", &code) == 1) {
		if (code >= 500) {
			printf("Error! Command failed!\n");
			exit(1);
		} else {
			return;
		}
	}
}

/* This functions opens the file, reads from socket and writes the file */

void get_file(int sd, char *f_name) {
	char buf[BUF_SIZE];
	int bytes_written = 1, bytes_read;
	FILE *fd;
	
	fd = fopen(f_name, "w");
	if (fd == NULL) {
		printf("Error! Couldn't create file!\n");
		exit(1);
	}
	
	while (bytes_written != 0) {
		bytes_read = recv(sd, buf, BUF_SIZE, 0);
		bytes_written = fwrite(buf, sizeof(char), bytes_read, fd);
	}
	if (fclose(fd) == EOF) {
		printf("Error! Couldn't save file!\n");
		exit(1);
	}
	
	printf("File saved sucessfully!\n");
	return; 
}
