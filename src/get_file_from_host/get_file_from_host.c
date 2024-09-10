#include "settings.h"
#include "get_file_from_host.h"
#include "tools.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

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

int connect_to_server(char *hostname)
{
	int				sock;
	struct sockaddr	*server;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	// config as a stream socket with connection through IPv4
	if (sock == -1) {
		perror("Error during socket creation");
		return -1;
	}
	server = get_server_socket_address(hostname);
	if (!server)
		return -1;
	if (connect(sock, server, sizeof(struct sockaddr_in)) < 0) {
		perror("Error during socket connection");
		free(server);
		return -1;
	}
	free(server);
	return sock;
}

int send_request(int sock, struct host_data *host_data)
{
	char	request[REQUEST_BUFFER_SIZE];

	printf("sending request, awaiting response ... ");
	snprintf(request, REQUEST_BUFFER_SIZE, // make sure that the size is limited
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

/**
 * @brief download a file pointed by an url
 * @param url Url of the file to download
 * @param storage_dir_path Optionnal directory path where to store the file
 * @param file_name Optionnal parameter to override the file name
 * @param bytes_per_sec Optionnal speed limit in bytes/s
 * @return A pointer to the file path.
 * The caller is responsible for freeing this memory.
 */
char	*get_file_from_host(char *url, char *storage_dir_path,
								char *file_name, unsigned long bytes_per_sec)
{
	int					sock;
	struct host_data	*host_data;
	char				*file_path;

	host_data = get_hostdata(url);
	if (!host_data)
		return NULL;
	if (!file_name)
		file_name = host_data->filename;
	sock = connect_to_server(host_data->hostname);
	file_path = get_file_path(file_name, storage_dir_path);
	if (sock == -1 || send_request(sock, host_data) ||
								download_file(sock, file_path, bytes_per_sec)) {
		free(file_path);
		free_hostdata(host_data);
		close(sock);
		return NULL;
	}
	free_hostdata(host_data);
	close(sock);
	return file_path;
}
