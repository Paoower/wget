#include "src.h"
#include "tools.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>

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
	char	response_merged[REQUEST_BUFFER_SIZE * 2];

	while ((*received = recv(sock, response, REQUEST_BUFFER_SIZE, 0)) > 0) {
		memmove(response_merged,
				response_merged + REQUEST_BUFFER_SIZE, REQUEST_BUFFER_SIZE);
		strncat(response_merged, response, *received);
		header_end = strstr(response_merged, "\r\n\r\n");
		if (header_end) {
			remaining_data_len = REQUEST_BUFFER_SIZE + *received
										- (header_end + 4 - response_merged);
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

struct timespec get_elapsed_time(struct timespec start_time)
{
	struct timespec	current_time;
	struct timespec	elapsed_time;

	clock_gettime(CLOCK_MONOTONIC, &current_time);
	if ((current_time.tv_nsec - start_time.tv_nsec) < 0) {
		elapsed_time.tv_sec = current_time.tv_sec - start_time.tv_sec - 1;
		elapsed_time.tv_nsec = current_time.tv_nsec
										+ 1000000000 - start_time.tv_nsec;
	} else {
		elapsed_time.tv_sec = current_time.tv_sec - start_time.tv_sec;
		elapsed_time.tv_nsec = current_time.tv_nsec - start_time.tv_nsec;
	}
	return elapsed_time;
}

void	limit_speed(struct timespec start_time,
							unsigned long int bytes_per_sec,
									unsigned long int total_bytes_downloaded)
{
	// calculer le temps passé depuis le début du téléchargement
	// avec le nombre de bytes deja téléchargés,
	// déduire le temps en trop pris par le programme
	// calculer enfin le temps nécessaire a ajouter pour la pause
	(void)start_time;
	(void)bytes_per_sec;
	(void)total_bytes_downloaded;
}

int write_data_into_file(int sock, FILE *fp, unsigned long int *bytes_per_sec)
{
	int					received;
	char				response[REQUEST_BUFFER_SIZE];
	int					remaining_data_len;
	struct timespec		start_time;
	struct timespec		elapsed_time;
	unsigned long int	total_bytes_downloaded;


	clock_gettime(CLOCK_MONOTONIC, &start_time);
	total_bytes_downloaded = 0;
	remaining_data_len = skip_htpp_header(sock, response, &received);
	if (remaining_data_len > 0)
		fwrite(response +
					received - remaining_data_len, 1, remaining_data_len, fp);
	else if (remaining_data_len < 0)
		return 1;
	while ((received = recv(sock, response, REQUEST_BUFFER_SIZE, 0)) > 0) {
		total_bytes_downloaded += received;
		fwrite(response, 1, received, fp);
		if (bytes_per_sec)
			limit_speed(start_time, *bytes_per_sec, total_bytes_downloaded);
	}
	if (received < 0) {
		perror("Error receiving data");
		return 1;
	}
	elapsed_time = get_elapsed_time(start_time);
	printf("elapsed time: %ld.%lu s\n", elapsed_time.tv_sec, elapsed_time.tv_nsec);
	printf("bytes downloaded: %lu\n", total_bytes_downloaded);
	return 0;
}

int download_file(int sock, char *dir_path,
						char *file_name, unsigned long int *bytes_per_sec)
{
	FILE	*fp;
	char	*file_path;

	file_path = get_file_path(file_name, dir_path);
	fp = fopen(file_path, "wb");
	if (fp == NULL) {
		perror("Error trying to open file");
		free(file_path);
		return 1;
	}
	if (write_data_into_file(sock, fp, bytes_per_sec)) {
		fclose(fp);
		free(file_path);
		return 1;
	}
	fclose(fp);
	free(file_path);
	return 0;
}
