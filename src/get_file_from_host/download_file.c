#define _GNU_SOURCE

#include "src.h"
#include "get_file_from_host.h"
#include "tools.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

void	limit_speed(struct timespec start_time,
							long unsigned bytes_per_sec,
									long unsigned total_bytes_downloaded)
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

int	write_data_into_file(int sock, FILE *fp, long unsigned *bytes_per_sec,
						struct timespec start_download_time, char *response,
										int received, int remaining_data_len)
{
	long unsigned	total_bytes_downloaded;

	total_bytes_downloaded = 0;
	if (remaining_data_len > 0) {
		total_bytes_downloaded += remaining_data_len;
		fwrite(response +
					received - remaining_data_len, 1, remaining_data_len, fp);
	}
	while ((received = recv(sock, response, REQUEST_BUFFER_SIZE, 0)) > 0) {
		total_bytes_downloaded += received;
		fwrite(response, 1, received, fp);
		if (*bytes_per_sec != 0 && received == REQUEST_BUFFER_SIZE)
			limit_speed(start_download_time,
									*bytes_per_sec, total_bytes_downloaded);
	}
	if (received < 0) {
		perror("Error receiving data");
		return 1;
	}
	return 0;
}

int download_file_without_header(int sock, FILE *fp, long unsigned *bytes_per_sec)
{
	int					received;
	char				response[REQUEST_BUFFER_SIZE];
	int					remaining_data_len;
	struct header_data	*header_data;
	struct timespec		start_download_time;

	clock_gettime(CLOCK_MONOTONIC, &start_download_time);
	header_data = skip_htpp_header(sock, response,
												&received, &remaining_data_len);
	if (header_data == NULL)
		return 1;
	printf("status %s\n", header_data->status);
	printf("content size: %s\n", header_data->content_size);
	if (write_data_into_file(sock, fp, bytes_per_sec,
				start_download_time, response, received, remaining_data_len))
		return 1;
	free_header_data(header_data);
	return 0;
}

int download_file(int sock, char *file_path, long unsigned *bytes_per_sec)
{
	FILE	*fp;

	fp = fopen(file_path, "wb");
	if (fp == NULL) {
		perror("Error trying to open file");
		return 1;
	}
	if (download_file_without_header(sock, fp, bytes_per_sec)) {
		fclose(fp);
		return 1;
	}
	fclose(fp);
	return 0;
}
