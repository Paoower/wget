#ifndef SRCS_H
# define SRCS_H

# define BUFFER_SIZE 1024
# define URL_REGEX "^((http|https)://)?(www\\.)?[a-zA-Z0-9\\-\\.]+\\.[a-zA-Z]{2,}(\\:[0-9]{1,5})?(/.*)?$"

struct	host_data {
	char	*hostname;
	char	*filepath;
	char	*filename;
};

int	download_file(char *url, char *filepath);

#endif
