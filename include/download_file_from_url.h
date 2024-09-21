#ifndef GET_FILE_FROM_HOST
# define GET_FILE_FROM_HOST

# include <openssl/ssl.h>
# define REQUEST_BUFFER_SIZE 1024

struct	host_data {
	char	*hostname;
	char	*filepath;
	char	*filename;
	int		is_secured;
};

struct header_data {
	int		remaining_data_length;
	char	*status;
	char	*content_size;
	char	*redirect_url;
};

struct file_data {
	struct header_data	*header_data;
	char				*file_path;
};

struct header_data	*download_file(int sock_fd, SSL *ssl,
								char *file_path, unsigned long bytes_per_sec);
char				*get_host_file_path(const char *storage_dir_path,
								char *file_name, struct host_data *host_data,
																int is_mirror);
struct host_data	*get_hostdata(char *url);
void				free_hostdata(struct host_data *host_data);

struct header_data	*skip_htpp_header(int sock_fd, SSL *ssl,
						char *response, int *received, int *remaining_data_len);
void				free_header_data(struct header_data *header_data);
#endif
