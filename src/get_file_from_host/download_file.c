#include "src.h"
#include "tools.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/stat.h>

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

	while ((*received = recv(sock, response, REQUEST_BUFFER_SIZE, 0)) > 0) {
		header_end = strstr(response, "\r\n\r\n");
		if (header_end) {
			remaining_data_len = *received - (header_end + 4 - response);
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

int write_data_into_file(int sock, FILE *fp)
{
	int		received;
	char	response[REQUEST_BUFFER_SIZE];
	int		remaining_data_len;

	remaining_data_len = skip_htpp_header(sock, response, &received);
	if (remaining_data_len > 0)
		fwrite(response +
					received - remaining_data_len, 1, remaining_data_len, fp);
	else if (remaining_data_len < 0)
		return 1;
	while ((received = recv(sock, response, REQUEST_BUFFER_SIZE, 0)) > 0)
		fwrite(response, 1, received, fp);
	if (received < 0) {
		perror("Error receiving data");
		return 1;
	}
	return 0;
}

int download_file(int sock, char *dir_path, char *file_name)
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
	if (write_data_into_file(sock, fp)) {
		fclose(fp);
		free(file_path);
		return 1;
	}
	fclose(fp);
	free(file_path);
	return 0;
}
