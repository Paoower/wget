#include "settings.h"
#include "src.h"
#include "tools.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>

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
 * @brief
 * Create directory if not exists and concatenate
 * the directory path with the file name to get the file path.
 * @param file_name The file name
 * @param dir_path Optionnal directory path
 * @return
 * Pointer to the allocated memory for the full file path.
 * The caller is responsible for freeing this memory.
 */
char	*get_file_path(const char *file_name, const char *dir_path) {
	struct stat	st;
	char		*file_path;

	if (!dir_path)
		return strdup(file_name);
	st = (struct stat){0};
	if (stat(dir_path, &st) == -1) {
		if (mkdir(dir_path, 0755) == -1) {
			perror("Error creation directory");
			return strdup(file_name);
		}
	}
	if (dir_path[strlen(dir_path) - 1] != '/')
		file_path = str_concat(dir_path, "/", file_name, NULL);
	else
		file_path = str_concat(dir_path, file_name, NULL);
	return file_path;
}

/**
 * @return
 * Remaining data length after the http header
 * or -1 if any error is triggered.
 */
int skip_htpp_header(int sock, char *response, int *received)
{
	char	*header_end;
	int		remaining_data_len;

	while ((*received = recv(sock, response, REQUEST_BUFFER_SIZE, 0)) > 0) {
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
	char	response[REQUEST_BUFFER_SIZE];
	int		remaining_data_len;

	remaining_data_len = skip_htpp_header(sock, response, &received);
	if (remaining_data_len > 0)
		fwrite(response +
				received - remaining_data_len, 1, remaining_data_len, fp);
	else if (remaining_data_len < 0)
		return 1;
	while ((received = recv(sock, response, REQUEST_BUFFER_SIZE, 0)) > 0)
		fwrite(response, 1, received, fp);
	if (received < 0) {
		perror("Error receiving data");
		return 1;
	}
	return 0;
}

int get_file(int sock,
		struct host_data *host_data, char *dir_path, char *file_name)
{
	FILE	*fp;
	char	*file_path;

	if (!file_name)
		file_name = host_data->filename;
	file_path = get_file_path(file_name, dir_path);
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

/**
 * @brief download a file pointed by an url
 * @param storage_dir_path Optionnal directory path where to store the file
 * @param file_name Optionnal parameter to override the file name
 * @return Returns 1 if an error occurs.
 */
int download_file(char *url, char *storage_dir_path, char *file_name)
{
	int					sock;
	struct host_data	*host_data;

	host_data = get_hostdata(url);
	if (!host_data)
		return 1;
	sock = connect_to_server(host_data->hostname);
	if (sock == -1 ||
			send_request(sock, host_data) ||
			get_file(sock, host_data, storage_dir_path, file_name)) {
		free_hostdata(host_data);
		close(sock);
		return 1;
	}
	free_hostdata(host_data);
	close(sock);
	return 0;
}
