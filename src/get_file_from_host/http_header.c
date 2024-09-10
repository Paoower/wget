#include "get_file_from_host.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

void	free_header_data(struct header_data *header_data)
{
	free(header_data->status);
	free(header_data);
}

struct header_data	*fill_http_data(char *http_header)
{
	struct header_data	*header_data;

	header_data = malloc(sizeof(struct header_data));
	header_data->status = get_http_response_info(http_header, "HTTP/1.1", " ");
	return header_data;
}

/**
 * @return
 * Remaining data length after the http header contained in response
 * or -1 if any error is triggered.
 */
struct header_data	*skip_htpp_header(int sock,
						char *response, int *received, int *remaining_data_len)
{
	char				*header_end;
	char				*response_merged;
	int					i;
	struct header_data	*header_data;

	response_merged = NULL;
	i = 1;
	while ((*received = recv(sock, response, REQUEST_BUFFER_SIZE, 0)) > 0) {
		response_merged = realloc(response_merged, REQUEST_BUFFER_SIZE * i);
		memset(response_merged + REQUEST_BUFFER_SIZE * (i - 1),
													0, REQUEST_BUFFER_SIZE);
		strncat(response_merged, response, *received);
		header_end = strstr(response_merged, "\r\n\r\n");
		if (header_end) {
			*remaining_data_len = response_merged + REQUEST_BUFFER_SIZE * i
															- (header_end + 4);
			header_data = fill_http_data(response_merged);
			free(response_merged);
			return header_data;
		}
		i++;
	}
	free(response_merged);
	if (*received < 0) {
		perror("Error receiving data");
		return NULL;
	}
	fprintf(stderr, "Data or http response not found");
	return NULL;
}
