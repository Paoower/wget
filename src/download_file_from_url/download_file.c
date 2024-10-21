#define _GNU_SOURCE

#include "src.h"
#include "download_file_from_url.h"
#include "tools.h"
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <openssl/err.h>
#include <arpa/inet.h>

static void	fill_basic_dl_data(struct dl_data *dl_data, int sock_fd,
								SSL *ssl, FILE *fp, unsigned long bytes_per_sec)
{
	dl_data->sock_fd = sock_fd;
	dl_data->ssl = ssl;
	dl_data->fp = fp;
	dl_data->bytes_per_sec = bytes_per_sec;
}

static void	limit_speed(int received, unsigned long bytes_per_sec)
{
	struct timespec	pause_time;
	double			time_in_seconds;

	time_in_seconds = received / (double)bytes_per_sec;
	pause_time.tv_sec = (long)time_in_seconds;
	pause_time.tv_nsec = (long)((time_in_seconds - pause_time.tv_sec) * 1e9);
	nanosleep(&pause_time, NULL);
}

// static int	init_chunk(char *data, int *data_size, struct dl_data *dld)
// {
// 	unsigned long	chunk_size;
// 	char			*endptr;
// 	int				i;

// 	i = 0;
// 	dld->chunk_data_count = 0;
// 	while (i + 1 < data_size) {
// 		if (data[i] == '\r' && data[i + 1] == '\n') {
// 			data[i] = '\0';
// 			chunk_size = strtol(*data, &endptr, 16);
// 			if (*data == endptr)
// 				return 0; // conversion error
// 			dld->is_in_chunk = true;
// 			memmove(data, data + i, *data_size - i);
// 			*data_size -= i;
// 			return chunk_size;
// 		}
// 		i++;
// 	}
// 	return 0;
// }

// /**
//  * @brief In chunk mode, find chunks in the buffer and write it
//  * in the file
//  */
// static void	handle_chunked_data(char *data,
// 								int *data_size, int *to_read, struct dl_data *dld)
// {
// 	int	current_chunk_len;

// new_chunk:
// 	if (*data_size <= 0)
// 		return;
// 	if (!dld->is_in_chunk) {
// 		dld->chunk_size = init_chunk(data, data_size, dld);
// 		if (dld->chunk_size == 0) {
// 			*data_size = 0;
// 			return;
// 		}
// 		// read chunk size and init variables
// 	}
// 	dld->chunk_data_count += *data_size;
// 	if (dld->chunk_data_count >= dld->chunk_size) {
// 	// if buffer contains end of the chunk
// 		dld->is_in_chunk = false;
// 		if (dld->chunk_data_count > dld->chunk_size) {
// 		// if buffer contains \r\n or next chunk
// 			current_chunk_len = *data_size -
// 									(dld->chunk_data_count - dld->chunk_size);
// 			dld->total_bytes_downloaded += current_chunk_len;
// 			fwrite(data, 1, current_chunk_len, dld->fp);
// 			*data += current_chunk_len + 2;
// 			*data_size -= current_chunk_len + 2; // ignore \r\n after data
// 			goto new_chunk;
// 		}
// 	}
// 	return;
// }

static int	write_data_into_file(struct dl_data *dld, char *response,
					int received, int remaining_data_len,
					struct header_data *hd, bool display, bool is_background)
{
	char	*data;
	// bool	is_chunked;
	int		to_write;
	int		to_read;

	// is_chunked = hd->transfer_encoding &&
	// 							strcmp(hd->transfer_encoding, "chunked") == 0;
	// dld->is_in_chunk = false;
	clock_gettime(CLOCK_MONOTONIC, &dld->start_download_time);
	dld->total_bytes_downloaded = 0;
	to_read = REQUEST_BUFFER_SIZE; // init variables
	if (remaining_data_len > 0) { // if there is data already received
		data = response + received - remaining_data_len;
		received = remaining_data_len;
		goto already_recv;
	}
	while ((received = read_http_data(dld->sock_fd, dld->ssl,
										response, to_read)) > 0) {
		data = response;
	already_recv:
		if (received > 0) {
			to_write = received;
			// if (is_chunked)
			// 	handle_chunked_data(data, &to_write, &to_read, dld);
			dld->total_bytes_downloaded += received;
			fwrite(data, 1, to_write, dld->fp);
			if (dld->bytes_per_sec > 0)
				limit_speed(received, dld->bytes_per_sec);
			update_bar(dld, hd->content_size, display, is_background);
		}
	}
	if (received < 0) {
		perror("Error receiving data");
		return 1;
	}
	return 0;
}

static void	print_download_infos(struct header_data *header_data,
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
								char *file_path, unsigned long bytes_per_sec,
								bool display, bool is_background)
{
	FILE				*fp;
	struct header_data	*hd;
	char				response[REQUEST_BUFFER_SIZE];
	int					received;
	int					remaining_data_len;
	struct dl_data		dl_data;

	if (!file_path)
		return NULL;
	hd = skip_htpp_header(sock_fd, ssl,
									response, &received, &remaining_data_len);
	if (!hd || !hd->status)
		goto err_exit;
	if (display)
		printf("status %d %s\n", hd->status_code, hd->status);
	if (is_redirect_status(hd->status_code))
		return hd;
	if (!is_ok_status(hd->status_code))
		goto err_exit;
	fp = fopen(file_path, "wb");
	if (fp == NULL) {
		perror("Error trying to open file");
		goto err_exit;
	}
	print_download_infos(hd, file_path, display);
	fill_basic_dl_data(&dl_data, sock_fd, ssl, fp, bytes_per_sec);
	write_data_into_file(&dl_data, response,
				received, remaining_data_len, hd, display, is_background);
	fclose(fp);
	return hd;
err_exit:
	free_header_data(hd);
	return NULL;
}
