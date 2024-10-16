#include "download_file_from_url.h"
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
	if (header_data) {
		free(header_data->status);
		free(header_data->content_size);
		free(header_data->redirect_url);
		free(header_data->transfer_encoding);
		free(header_data);
	}
}

static void	fill_status(char *http_header, struct header_data *header_data)
{
	char	*status_str;
	char	*mid_pos;

	header_data->status = NULL;
	header_data->status_code = 0;
	status_str = get_http_response_info(http_header, "HTTP/1.1", " ");
	if (!status_str)
		return;
	header_data->status_code = atoi(status_str);
	mid_pos = strchr(status_str, ' ');
	if (!mid_pos) {
		header_data->status = strdup("");
		free(status_str);
		return;
	}
	header_data->status = strdup(mid_pos + 1);
	free(status_str);
}

static struct header_data	*fill_http_data(char *http_header)
{
	struct header_data	*header_data;
	char				*content_type;

	header_data = malloc(sizeof(struct header_data));
	if (!header_data)
		return NULL;
	fill_status(http_header, header_data);
	header_data->content_size = get_http_response_info(
										http_header, "content-length", NULL);
	header_data->redirect_url = get_http_response_info(
										http_header, "location", NULL);
	header_data->transfer_encoding = get_http_response_info(
										http_header, "transfer-encoding", NULL);
	content_type = get_http_response_info(http_header, "content-type", NULL);
	if (content_type && strncmp(content_type, "text/html", 9) == 0) {
		header_data->is_html = true;
	} else
		header_data->is_html = false;
	free(content_type);
	return header_data;
}

static char	*concat_header(char *response, char *response_merged,
												int received, int header_size)
{
	char	*new_response_merged;
	char	*response_merged_end;

	new_response_merged = realloc(response_merged, header_size + 1);
	if (!new_response_merged) {
		perror("Memory reallocation failed");
		free(response_merged);
		return NULL;
	}
	response_merged = new_response_merged;
	response_merged_end = response_merged + header_size;
	memset(response_merged_end - received, 0, received + 1);
	memcpy(response_merged_end - received, response, received);
	return response_merged;
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
	struct header_data	*header_data;
	int					header_size;
	char				*header_finish_pattern;


	response_merged = NULL;
	header_size = 0;
	header_finish_pattern = "\r\n\r\n";
	while ((*received = read_http_data(sock_fd, ssl,
										response, REQUEST_BUFFER_SIZE)) > 0) {
		header_size += *received;
		response_merged = concat_header(response,
										response_merged, *received, header_size);
		if (!response_merged)
			return NULL;
		header_end = strstr(response_merged, header_finish_pattern);
		if (header_end) {
			// printf("\nHEADER\n%s\n\n", response_merged);
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
