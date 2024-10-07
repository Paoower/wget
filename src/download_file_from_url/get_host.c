#include "download_file_from_url.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_hostdata(struct host_data *host_data)
{
	if (!host_data)
		return;
	free(host_data->hostname);
	free(host_data->filename);
	free(host_data->filepath);
	free(host_data->dirpath);
	free(host_data);
}

char	*get_host_file_path(const char *storage_dir_path,
					char *file_name, struct host_data *host_data, int is_mirror)
{
	char	*new_storage_dir_path;
	char	*file_path;

	if (!file_name)
		file_name = host_data->filename;
	if (is_mirror) {
		if (storage_dir_path)
			new_storage_dir_path = str_concat(storage_dir_path,
					"/", host_data->hostname, "/", host_data->dirpath, NULL);
		else
			new_storage_dir_path = str_concat(host_data->hostname,
					"/", host_data->dirpath, NULL);
		if (new_storage_dir_path) {
			file_path = get_file_path(file_name, new_storage_dir_path);
			free(new_storage_dir_path);
			return file_path;
		}
	}
	return get_file_path(file_name, storage_dir_path);
}

struct host_data *get_hostdata(char *url)
{
	struct host_data *host_data;
	char *protocol_end, *hostname_end;

	if (!url)
		return NULL;
	host_data = malloc(sizeof(struct host_data));
	if (!host_data) {
		perror("Memory allocation failed");
		return NULL;
	}

	// Check if the URL is secured (HTTPS)
	if (strncmp(url, "https://", 8) == 0) {
		host_data->is_secured = 1;
		protocol_end = url + 8;
	} else if (strncmp(url, "http://", 7) == 0) {
		host_data->is_secured = 0;
		protocol_end = url + 7;
	} else {
		// Default to HTTP if no protocol is specified
		host_data->is_secured = 0;
		protocol_end = url;
	}

	// Find end of hostname
	hostname_end = strchr(protocol_end, '/');
	if (!hostname_end) {
		// URL doesn't contain a path, just a hostname
		host_data->hostname = strdup(protocol_end);
		host_data->filepath = strdup("/");
	} else {
		// Extract hostname
		host_data->hostname = strndup(protocol_end, hostname_end - protocol_end);

		// Extract filepath
		host_data->filepath = strdup(hostname_end);
	}

	// Extract filename (last part of the filepath)
	char *filename_start = strrchr(host_data->filepath, '/');
	if (filename_start && *(filename_start + 1) != '\0') {
		host_data->filename = strdup(filename_start + 1);
		host_data->dirpath = strndup(host_data->filepath + 1,
							(filename_start + 1) - (host_data->filepath + 1));
	} else {
		// If no filename is found, use a default name
		host_data->filename = strdup("index.html");
		host_data->dirpath = strdup("");
	}

	// Check if memory allocation was successful
	if (!host_data->hostname || !host_data->filepath || !host_data->filename) {
		perror("Memory allocation failed");
		free_hostdata(host_data);
		return NULL;
	}

	return host_data;
}
