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

static void	write_data_into_file_core(struct dl_data *dld, char *response,
					int received, char *data,
					struct header_data *hd, bool display, bool is_background)
{
	char	prev_buf[REQUEST_BUFFER_SIZE];
	int		prev_buf_len;
	bool	is_first_read;
	bool	is_first_buf;

	prev_buf_len = 0;
	is_first_read = true;
	is_first_buf = true;
	if (data && received > 0)
		goto already_recv;
	while ((received = read_http_data(dld->sock_fd, dld->ssl,
										response, REQUEST_BUFFER_SIZE)) > 0) {
		data = response;
	already_recv:
		dld->total_bytes_downloaded += received;
		if (dld->is_chunked)
			write_chunked(dld, prev_buf, &prev_buf_len,
							data, received, &is_first_read, &is_first_buf);
		else
			fwrite(data, 1, received, dld->fp);
		if (dld->bytes_per_sec > 0)
			limit_speed(received, dld->bytes_per_sec);
		update_bar(dld, hd->content_size, display, is_background);
	}
	if (dld->is_chunked && !is_first_read) // write the last buffer
		write_chunked(dld, prev_buf, &prev_buf_len,
							NULL, 0, &is_first_read, &is_first_buf);
}

static int	write_data_into_file(struct dl_data *dld, char *response,
					int received, int remaining_data_len,
					struct header_data *hd, bool display, bool is_background)
{
	char	*data;

	dld->is_chunked = hd->transfer_encoding &&
								strcmp(hd->transfer_encoding, "chunked") == 0;
	clock_gettime(CLOCK_MONOTONIC, &dld->start_download_time);
	dld->total_bytes_downloaded = 0;
	data = NULL;
	// init variables
	if (remaining_data_len > 0) { // if there is data already received
		data = response + received - remaining_data_len;
		received = remaining_data_len;
	}
	write_data_into_file_core(dld, response, received, data, hd,
													display, is_background);
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
