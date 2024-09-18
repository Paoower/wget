#include "get_file_from_host.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <arpa/inet.h>

void	free_header_data(struct header_data *header_data)
{
	free(header_data->status);
	free(header_data->content_size);
	free(header_data);
}

struct header_data	*fill_http_data(char *http_header)
{
	struct header_data	*header_data;

	header_data = malloc(sizeof(struct header_data));
	header_data->status = get_http_response_info(http_header, "HTTP/1.1", " ");
	header_data->content_size = get_http_response_info(http_header,
													"Content-Length", NULL);
	return header_data;
}

/**
 * @return
 * Remaining data length after the http header contained in response
 * or -1 if any error is triggered.
 */
struct header_data	*skip_htpp_header(int sock_fd, SSL *ssl,
						char *response, int *received, int *remaining_data_len)
{
	char				*header_end;
	char				*response_merged;
	char				*new_response_merged;
	struct header_data	*header_data;
	int					header_size;
	char				*header_finish_pattern;

	response_merged = NULL;
	header_size = 0;
	header_finish_pattern = "\r\n\r\n";
	while ((*received = read_http_data(sock_fd, ssl,
										response, REQUEST_BUFFER_SIZE)) > 0) {
		header_size += *received;
		new_response_merged = realloc(response_merged, header_size);
		if (!new_response_merged) {
			perror("Memory reallocation failed");
			free(response_merged);
			return NULL;
		}
		response_merged = new_response_merged;
		memset(response_merged + header_size - *received, 0, *received);
		strncat(response_merged, response, *received);
		header_end = strstr(response_merged, header_finish_pattern);
		if (header_end) {
			// printf("\nHEADER:\n%s\n\n", response_merged);
			*remaining_data_len = response_merged + header_size
								- (header_end + strlen(header_finish_pattern));
			header_data = fill_http_data(response_merged);
			free(response_merged);
			return header_data;
		}
	}
	free(response_merged);
	if (*received < 0) {
		perror("Error receiving data");
		return NULL;
	}
	fprintf(stderr, "Data or http response not found");
	return NULL;
}
