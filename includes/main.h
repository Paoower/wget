#ifndef MAIN_H
# define MAIN_H

#define BUFFER_SIZE 1024

struct	host_data {
	char	*hostname;
	char	*filepath;
	char	*filename;
};

struct host_data	*get_hostname_and_filepath(char *url);
struct sockaddr	*get_server_socket_address(char *hostname);
int	connect_to_server(char *hostname);
int	send_request(int sock, struct host_data *host_data);
int	write_data_into_file(int sock, struct host_data *host_data);
int	download_file(char *url);

#endif
