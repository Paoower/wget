#include "tools.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
