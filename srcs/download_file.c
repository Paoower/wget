#include "srcs.h"
#include "tools.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

struct host_data	*get_hostdata(char *url)
{
	struct host_data	*server_data;

	if (!does_match_with_pattern(url, URL_REGEX)) {
		fprintf(stderr, "Error : not an url (%s)\n", url);
		return NULL;
	}
	server_data = malloc(sizeof(struct host_data));
	if (!server_data) {
		perror("Memory allocation failed");
		return NULL;
	}
	server_data->hostname = strdup("pbs.twimg.com");
	server_data->filename = strdup("EMtmPFLWkAA8CIS.jpg");
	server_data->filepath = strdup("/media/EMtmPFLWkAA8CIS.jpg");
	if (!server_data->hostname ||
			!server_data->filename || !server_data->hostname) {
		free(server_data->hostname);
		free(server_data->filename);
		free(server_data->filepath);
		free(server_data);
		return NULL;
	}
	return server_data;
}

struct sockaddr	*get_server_socket_address(char *hostname)
{
	struct hostent		*host;
	struct sockaddr_in	*server;

	host = gethostbyname(hostname);
	if (!host) {
		fprintf(stderr, "Error : Host not found (%s)\n", hostname);
		return NULL;
	}
	server = malloc(sizeof(struct sockaddr_in));
	if (!server) {
		perror("Failed to allocate memory for strings");
		return NULL;
	}
	server->sin_addr = *(struct in_addr *)host->h_addr_list[0];
	// use the host first IP address
	server->sin_family = AF_INET; // define type of IP (IPv4)
	server->sin_port = htons(80); // default port for HTTP
	return (struct sockaddr *)server;
}

int	connect_to_server(char *hostname)
{
	int					sock;
	struct sockaddr		*server;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	// config as a stream socket with connection through IPv4
	if (sock == -1) {
		perror("Error during socket creation");
		return -1;
	}
	server = get_server_socket_address(hostname);
	if (!server)
		return -1;
	if (connect(sock, server, sizeof(server)) < 0) {
		perror("Error during socket connection");
		return -1;
	}
	return sock;
}

int	send_request(int sock, struct host_data *host_data)
{
	char	request[BUFFER_SIZE];

	snprintf(request, BUFFER_SIZE, // make sure that the size is limited
			"GET %s HTTP/1.1\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n\r\n", // default format for http request
			host_data->filepath, host_data->hostname);
	if (send(sock, request, strlen(request), 0) < 0) {
		perror("Error at sending request");
		return 1;
	}
	return 0;
}

int	write_data_into_file(int sock, struct host_data *host_data, char *path)
{
	FILE	*fp;
	int		received;
	char	response[BUFFER_SIZE];

	fp = fopen(strcat(path, host_data->filename), "wb");
	if (fp == NULL) {
		perror("Error trying to open file");
		return 1;
	}
	while ((received = recv(sock, response, BUFFER_SIZE, 0)) > 0)
		fwrite(response, 1, received, fp);
	if (received < 0) {
		perror("Error receiving data");
		return 1;
	}
	fclose(fp);
	return 0;
}

int	download_file(char *url, char *filepath)
{
	int					sock;
	struct host_data	*host_data;

	host_data = get_hostdata(url);
	if (!host_data)
		return 1;
	sock = connect_to_server(host_data->hostname);
	if (sock == -1)
		return 1;
	if (send_request(sock, host_data))
		return 1;
	if (write_data_into_file(sock, host_data, filepath))
		return 1;
	close(sock);
	return 0;
}
