#include "main.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

struct host_data	*get_hostname_and_filepath(char *url) {
	struct host_data	server_data;

	// format
	return &server_data;
}

struct sockaddr	*get_server_socket_address(char *hostname)
{
	struct hostent		*host;
	struct sockaddr_in	server;

	host = gethostbyname(hostname);
	if (host == NULL) {
		fprintf(stderr, "Error : Host not found (%s)\n", hostname);
		return NULL;
	}
	server.sin_addr = *(struct in_addr *)host->h_addr_list[0];
	// use the host first IP address
	server.sin_family = AF_INET; // define type of IP (IPv4)
	server.sin_port = htons(80); // default port for HTTP
	return (struct sockaddr *)&server;
}

int	connect_to_server(char *hostname)
{
	int					sock;
	struct sockaddr_in	*server;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	// config as a stream socket with connection through IPv4
	if (sock == -1) {
		perror("Error during socket creation");
		return NULL;
	}
	server = get_server_socket_address(hostname);
	if (server == NULL)
		return NULL;
	if (connect(sock, server, sizeof(server)) < 0) {
		perror("Error during socket connection");
		return NULL;
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

int	write_data_into_file(int sock, struct host_data *host_data)
{
	FILE	*fp;
	int		received;
	char	response[BUFFER_SIZE];

	fp = fopen(host_data->filename, "wb");
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

int	download_file(char *url)
{
	int					sock;
	struct host_data	*host_data;

	host_data = get_hostname_and_filepath(url);
	sock = connect_to_server(host_data->hostname);
	if (!sock)
		return 1;
	if (send_request(sock, host_data))
		return 1;
	if (write_data_into_file(sock, host_data))
		return 1;
	close(sock);
	return 0;
}
