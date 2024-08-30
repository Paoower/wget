#ifndef SRCS_H
# define SRCS_H

# define BUFFER_SIZE 1024
# define URL_REGEX "^((http|https)://)?(www\\.)?[a-zA-Z0-9\\-\\.]+\\.[a-zA-Z]{2,}(\\:[0-9]{1,5})?(/.*)?$"

struct	host_data {
	char	*hostname;
	char	*filepath;
	char	*filename;
};

struct host_data	*get_hostdata(char *url);
struct sockaddr		*get_server_socket_address(char *hostname);
int					connect_to_server(char *hostname);
int					send_request(int sock, struct host_data *host_data);
int					write_data_into_file(int sock, struct host_data *host_data, char *path);
int					download_file(char *url, char *filepath);

#endif
