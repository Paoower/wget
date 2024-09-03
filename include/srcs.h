#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef SRCS_H
# define SRCS_H

# define BUFFER_SIZE 1024
# define URL_REGEX "^((http|https)://)?(www\\.)?[a-zA-Z0-9\\-\\.]+\\.[a-zA-Z]{2,}(\\:[0-9]{1,5})?(/.*)?$"
# define DATA_PATH "./data/"

struct	host_data {
	char	*hostname;
	char	*filepath;
	char	*filename;
};

// Download file
int					download_file(char *url, char *storage_dir_path, char *file_name);
struct host_data	*get_hostdata(char *url, char *file_name);
void				free_hostdata(struct host_data *host_data);

// Arguments handling
int handle_args(int argc, char *argv[]);

#endif
