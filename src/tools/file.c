#include "tools.h"
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

static char	*expand_tilde(const char *dir_path)
{
	const char	*home;

	if (dir_path[0] == '~') {
		home = getenv("HOME");
		if (home) {
			return str_concat(home, dir_path + 1, NULL);
		}
	}
	return strdup(dir_path);
}

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
	char		*full_dir_path;

	if (!dir_path)
		return strdup(file_name);
	full_dir_path = expand_tilde(dir_path);
	st = (struct stat){0};
	if (stat(full_dir_path, &st) == -1) {
		if (mkdir(full_dir_path, 0755) == -1) {
			perror("Error creation directory");
			free(full_dir_path);
			return strdup(file_name);
		}
	}
	if (full_dir_path[strlen(full_dir_path) - 1] != '/')
		file_path = str_concat(full_dir_path, "/", file_name, NULL);
	else
		file_path = str_concat(full_dir_path, file_name, NULL);
	free(full_dir_path);
	return file_path;
}
