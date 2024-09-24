#define _GNU_SOURCE

#include "src.h"
#include "download_file_from_url.h"
#include "tools.h"
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <openssl/err.h>
#include <arpa/inet.h>

void	limit_speed(struct timespec start_time,
										unsigned long bytes_per_sec,
										unsigned long total_bytes_downloaded)
{
	struct timespec	elapsed_time;
	long double		expected_download_time;
	struct timespec	expected_download_time_s;
	struct timespec	pause_time;

	// elapsed time from the start of the download
	elapsed_time = get_elapsed_time(start_time);

	// time that the prgm should take to download the current
	// total_bytes_downloaded + next buffer size
	// at a certain rate (bytes_per_sec)
	expected_download_time = (long double)(total_bytes_downloaded +
						REQUEST_BUFFER_SIZE) / (long double)bytes_per_sec;
	expected_download_time_s.tv_sec = (long)expected_download_time;
	expected_download_time_s.tv_nsec = (long)((expected_download_time
						- (long double)expected_download_time_s.tv_sec) * 1e9);

	pause_time = time_diff(elapsed_time, expected_download_time_s);
	if (pause_time.tv_sec >= 0)
		nanosleep(&pause_time, NULL);
}

void	update_bar(unsigned long total_bytes_downloaded,
											char *content_size, bool display)
{
	int		content_size_f;
	int		bar_width;
	float	percentage;
	int		position;

	if (!display || !content_size)
		return;
	content_size_f = atof(content_size);
	bar_width = 50;
	percentage = ((float)total_bytes_downloaded / content_size_f) * 100;
	position = bar_width * percentage / 100;
	printf("[");
	for (int i = 0; i < bar_width; ++i) {
		if (i < position)
			printf("=");
		else if (i == position)
			printf(">");
		else
			printf(" ");
	}
	printf("] %.2f%%\r", percentage);
	fflush(stdout);
}

/*
4\r\n    <-- Taille du chunk (4 octets)
Wiki\r\n <-- Données du chunk (taille 4 octets)
5\r\n    <-- Taille du chunk (5 octets)
pedia\r\n <-- Données du chunk (taille 5 octets)
0\r\n    <-- Fin des chunks
\r\n     <-- Fin de la réponse
la size est en hexa
*/

char	*get_chunked_data(int sock_fd, SSL *ssl, char *data, int *received)
{
	long	chunk_size;
	char	*endptr;
	char	*chunk;

	chunk_size = strtol(data, &endptr, 16);
	if (data == endptr)
		return NULL; // conversion error
	if (chunk_size == 0) {
		*received = 0;
		return data; // end of data
	}
	chunk = malloc(chunk_size);
	if (!chunk)
		return NULL;
	memcpy(chunk, endptr + 2, *received); // ignore "\r\n" after the chunk size
	*received = read_http_data(sock_fd, ssl, chunk, chunk_size - *received);
	return chunk;
}

int	write_data_into_file(int sock_fd, SSL *ssl, FILE *fp,
		unsigned long bytes_per_sec, char *response, int received,
		int remaining_data_len, struct header_data *header_data, bool display)
{
	unsigned long	total_bytes_downloaded;
	struct timespec	start_download_time;
	char			*data;
	bool			is_chunked;

	is_chunked = header_data->transfer_encoding &&
						strcmp(header_data->transfer_encoding, "chunked") == 0;
	clock_gettime(CLOCK_MONOTONIC, &start_download_time);
	total_bytes_downloaded = 0;
	if (remaining_data_len > 0) {
		data = response + received - remaining_data_len;
		received = remaining_data_len;
		goto already_recv;
	}
	while ((received = read_http_data(sock_fd, ssl,
										response, REQUEST_BUFFER_SIZE)) > 0) {
		data = response;
	already_recv:
		if (is_chunked) {
			data = get_chunked_data(sock_fd, ssl, data, &received);
			if (!data)
				return 1;
		}
		total_bytes_downloaded += received;
		fwrite(data, 1, received, fp);
		if (bytes_per_sec > 0 && received == REQUEST_BUFFER_SIZE)
			limit_speed(start_download_time,
									bytes_per_sec, total_bytes_downloaded);
		update_bar(total_bytes_downloaded, header_data->content_size, display);
		if (is_chunked)
			free(data);
	}
	if (received < 0) {
		perror("Error receiving data");
		return 1;
	}
	return 0;
}

void	print_download_infos(struct header_data *header_data,
												char *file_path, bool display)
{
	if (!display || !header_data)
		return;
	if (header_data->content_size)
		printf("content size: %s [~%.2fMB]\n", header_data->content_size,
						bytes_to_megabytes(atoi(header_data->content_size)));
	else
		printf("content size: unspecified\n");
	printf("saving file to: %s\n", file_path);
}

/**
 * @brief
 * Open or create a file, read data from sock_fd or ssl and write it into file
 * @param sock_fd socket fd used by the http connection
 * @param ssl optionnal ssl pointer used by the http connection
 * @param file_path path of the file to download
 * @param bytes_per_sec the download speed limit in byte per secondes
 * @return
 * A pointer to the header_data, or NULL
 */
struct header_data	*download_file(int sock_fd, SSL *ssl,
					char *file_path, unsigned long bytes_per_sec, bool display)
{
	FILE				*fp;
	struct header_data	*header_data;
	char				response[REQUEST_BUFFER_SIZE];
	int					received;
	int					remaining_data_len;

	if (!file_path)
		return NULL;
	header_data = skip_htpp_header(sock_fd, ssl,
									response, &received, &remaining_data_len);
	if (!header_data || !header_data->status)
		goto err_exit;
	if (display)
		printf("status %s\n", header_data->status);
	if (is_redirect_status(header_data->status))
		return header_data;
	if (!is_ok_status(header_data->status))
		goto err_exit;
	fp = fopen(file_path, "wb");
	if (fp == NULL) {
		perror("Error trying to open file");
		goto err_exit;
	}
	print_download_infos(header_data, file_path, display);
	write_data_into_file(sock_fd, ssl, fp, bytes_per_sec, response,
							received, remaining_data_len, header_data, display);
	fclose(fp);
	return header_data;
err_exit:
	free_header_data(header_data);
	return NULL;
}
