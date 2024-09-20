#include "get_file_from_host.h"
#include "tools.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <openssl/err.h>
#include <arpa/inet.h>

void	cleanup(SSL *ssl, SSL_CTX *ctx, char *file_path,
									struct host_data *host_data, int sock_fd)
{
	if (ssl) {
		SSL_shutdown(ssl);
		SSL_free(ssl);
	}
	if (ctx)
		SSL_CTX_free(ctx);
	EVP_cleanup();
	if (file_path)
		free(file_path);
	if (host_data)
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

int send_request(int sock_fd, SSL *ssl, struct host_data *host_data)
{
	char	request[REQUEST_BUFFER_SIZE];

	printf("sending request, awaiting response ... ");
	snprintf(request, REQUEST_BUFFER_SIZE, // make sure that the size is limited
			"GET %s HTTP/1.1\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n\r\n", // default format for http request
			host_data->filepath, host_data->hostname);
	if (send_http_request(sock_fd, ssl, request, strlen(request)) < 0) {
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
char	*get_file_from_host(char *url, const char *storage_dir_path,
					char *file_name, unsigned long bytes_per_sec, int is_mirror)
{
	int					sock_fd;
	struct host_data	*host_data;
	char				*file_path;
	SSL_CTX				*ctx;
	SSL					*ssl;
	struct header_data	*header_data;

	ssl = NULL;
	ctx = NULL;
	host_data = get_hostdata(url);
	if (!host_data)
		return NULL;
	sock_fd = connect_to_server(host_data->hostname, host_data->is_secured);
	if (host_data->is_secured) {
		ssl = create_ssl_connection(&ctx, sock_fd);
		if (!ssl) {
			cleanup(NULL, NULL, NULL, host_data, sock_fd);
			return NULL;
		}
	}
	file_path = get_host_file_path(storage_dir_path,
											file_name, host_data, is_mirror);
	if (sock_fd == -1 || send_request(sock_fd, ssl, host_data)) {
		cleanup(ssl, ctx, file_path, host_data, sock_fd);
		return NULL;
	}
	header_data = download_file(sock_fd, ssl, file_path, bytes_per_sec);
	if (strcmp(header_data->status, "301 Moved Permanently") == 0 ||
					strcmp(header_data->status, "302 Moved Temporarily") == 0) {
		cleanup(ssl, ctx, file_path, host_data, sock_fd);
		file_path = get_file_from_host(header_data->redirect_url,
						storage_dir_path, file_name, bytes_per_sec, is_mirror);
		free_header_data(header_data); // this sucks,
		// memory is erased only when the final link is found
		return file_path;
	}
	free_header_data(header_data);
	cleanup(ssl, ctx, NULL, host_data, sock_fd);
	return file_path;
}
