#include "srcs.h"
#include "tools.h"
#include <unistd.h>
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

// return the remaining data len after the http header
// or -1 if any error is triggered
int skip_htpp_header(int sock, char *response, int *received)
{
	char	*header_end;
	int		remaining_data_len;

	while ((*received = recv(sock, response, BUFFER_SIZE, 0)) > 0) {
		header_end = strstr(response, "\r\n\r\n");
		if (header_end) {
			remaining_data_len = *received - (header_end + 4 - response);
			return remaining_data_len;
		}
	}
	if (*received < 0) {
		perror("Error receiving data");
		return -1;
	}
	fprintf(stderr, "No http header found");
	return -1;
}

int write_data_into_file(int sock, FILE *fp)
{
	int		received;
	char	response[BUFFER_SIZE];
	int		remaining_data_len;

	remaining_data_len = skip_htpp_header(sock, response, &received);
	if (remaining_data_len > 0)
		fwrite(response +
				received - remaining_data_len, 1, remaining_data_len, fp);
	else if (remaining_data_len < 0)
		return 1;
	while ((received = recv(sock, response, BUFFER_SIZE, 0)) > 0)
		fwrite(response, 1, received, fp);
	if (received < 0) {
		perror("Error receiving data");
		return 1;
	}
	return 0;
}

int get_file(int sock, struct host_data *host_data, char *path)
{
	FILE	*fp;
	char	*file_path;

	file_path = concat(path, host_data->filename);
	printf("%s\n", file_path);
	fp = fopen(file_path, "wb");
	if (fp == NULL) {
		perror("Error trying to open file");
		free(file_path);
		return 1;
	}
	if (write_data_into_file(sock, fp)) {
		fclose(fp);
		free(file_path);
		return 1;
	}
	fclose(fp);
	free(file_path);
	return 0;
}

int download_file(char *url, char *storage_dir_path, char *file_name)
{
	int					sock;
	struct host_data	*host_data;

	host_data = get_hostdata(url, file_name);
	if (!host_data)
		return 1;
	sock = connect_to_server(host_data->hostname);
	if (sock == -1 ||
			send_request(sock, host_data) ||
			get_file(sock, host_data, storage_dir_path)) {
		free_hostdata(host_data);
		close(sock);
		return 1;
	}
	free_hostdata(host_data);
	close(sock);
	return 0;
}
