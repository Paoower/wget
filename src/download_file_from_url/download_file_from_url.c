#include "download_file_from_url.h"
#include "tools.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <openssl/err.h>
#include <arpa/inet.h>

void	free_file_data(struct file_data *file_data)
{
	if (file_data) {
		free_hostdata(file_data->host_data);
		free_header_data(file_data->header_data);
		free(file_data->file_path);
		free(file_data);
	}
}

void	cleanup(SSL *ssl, SSL_CTX *ctx, int sock_fd,
											struct host_data *host_data)
{
	if (ssl) {
		SSL_shutdown(ssl);
		SSL_free(ssl);
	}
	if (ctx)
		SSL_CTX_free(ctx);
	EVP_cleanup();
	free_hostdata(host_data);
	if (sock_fd != -1)
		close(sock_fd);
}

SSL	*create_ssl_connection(SSL_CTX **ctx, int sock_fd)
{
	SSL	*ssl;

	SSL_library_init();
	SSL_load_error_strings(); // give precise error descriptions
	OpenSSL_add_ssl_algorithms(); // init crypto lib used by ssl
	*ctx = SSL_CTX_new(TLS_client_method());
	if (!*ctx) {
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	ssl = SSL_new(*ctx);
	if (!ssl) {
		ERR_print_errors_fp(stderr);
		SSL_CTX_free(*ctx);
		return NULL;
	}
	SSL_set_fd(ssl, sock_fd);
	if (SSL_connect(ssl) <= 0) {
		ERR_print_errors_fp(stderr);
		SSL_free(ssl);
		SSL_CTX_free(*ctx);
		return NULL;
	}
	return ssl;
}

struct sockaddr	*get_server_socket_address(char *hostname, int is_secured)
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
	server->sin_port = is_secured ? htons(443) : htons(80);
	// default port for HTTPS AND HTTP
	return (struct sockaddr *)server;
}

int connect_to_server(char *hostname, int is_secured)
{
	int				sock;
	struct sockaddr	*server;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	// config as a stream socket with connection through IPv4
	if (sock == -1) {
		perror("Error during socket creation");
		return -1;
	}
	server = get_server_socket_address(hostname, is_secured);
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

int send_request(int sock_fd, SSL *ssl,
									struct host_data *host_data, bool display)
{
	char	request[REQUEST_BUFFER_SIZE];
	if (display)
		printf("sending request, awaiting response ... ");
	snprintf(request, REQUEST_BUFFER_SIZE, // make sure that the size is limited
			"GET %s HTTP/1.1\r\n"
			"Host: %s\r\n"
			"User-Agent: Wbaget/1.0\r\n"
			"Connection: close\r\n\r\n", // default format for http request
			host_data->filepath, host_data->hostname);
	if (send_http_request(sock_fd, ssl, request, strlen(request)) < 0) {
		perror("Error at sending request");
		return 1;
	}
	return 0;
}

struct file_data	*request_and_download_file(int sock_fd, SSL *ssl,
					SSL_CTX *ctx, struct host_data *host_data,
					const char *storage_dir_path, char *file_name,
					unsigned long bytes_per_sec, int is_mirror, bool display)
{
	struct file_data	*file_data;

	if (send_request(sock_fd, ssl, host_data, display)) {
		cleanup(ssl, ctx, sock_fd, host_data);
		return NULL;
	}
	file_data = malloc(sizeof(struct file_data));
	if (!file_data) {
		cleanup(ssl, ctx, sock_fd, host_data);
		return NULL;
	}
	file_data->file_path = get_host_file_path(storage_dir_path,
											file_name, host_data, is_mirror);
	file_data->header_data = download_file(sock_fd, ssl,
								file_data->file_path, bytes_per_sec, display);
	cleanup(ssl, ctx, sock_fd, NULL);
	return file_data;
}

struct file_data	*download_file_from_url_core(char *url,
					const char *storage_dir_path, char *file_name,
					unsigned long bytes_per_sec, int is_mirror, bool display)
{
	int					sock_fd;
	struct file_data	*file_data;
	struct host_data	*host_data;
	SSL_CTX				*ctx;
	SSL					*ssl;

	ssl = NULL;
	ctx = NULL;
	if (!(host_data = get_hostdata(url)))
		return NULL;
	sock_fd = connect_to_server(host_data->hostname, host_data->is_secured);
	if (sock_fd == -1) {
		cleanup(NULL, NULL, sock_fd, host_data);
		return NULL;
	}
	if (host_data->is_secured) {
		ssl = create_ssl_connection(&ctx, sock_fd);
		if (!ssl) {
			cleanup(NULL, NULL, sock_fd, host_data);
			return NULL;
		}
	}
	file_data = request_and_download_file(sock_fd, ssl, ctx, host_data,
			storage_dir_path, file_name, bytes_per_sec, is_mirror, display);
	file_data->host_data = host_data;
	return file_data;
}

/**
 * @brief download a file pointed by an url
 * @param url Url of the file to download
 * @param storage_dir_path Optionnal directory path where to store the file
 * @param file_name Optionnal parameter to override the file name
 * @param bytes_per_sec Optionnal speed limit in bytes/s
 * @return A pointer to the file path.
 * The caller is responsible for freeing this memory with `free_file_data`.
 */
struct file_data	*download_file_from_url(char *url,
					const char *storage_dir_path, char *file_name,
					unsigned long bytes_per_sec, int is_mirror, bool display)
{
	struct file_data	*file_data;
	struct file_data	*new_file_data;
	struct header_data	*hd;

	file_data = download_file_from_url_core(url, storage_dir_path,
								file_name, bytes_per_sec, is_mirror, display);
	while (file_data && (hd = file_data->header_data) &&
											is_redirect_status(hd->status)) {
		if (display)
			printf("\nRedirect to \"%s\"\n", hd->redirect_url);
		new_file_data = download_file_from_url_core(hd->redirect_url,
				storage_dir_path, file_name, bytes_per_sec, is_mirror, display);
		free_file_data(file_data);
		file_data = new_file_data;
	}
	if (!file_data || !(hd = file_data->header_data) ||
							!is_ok_status(hd->status)) {
		free_file_data(file_data);
		return NULL;
	}
	return file_data;
}
