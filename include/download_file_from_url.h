#ifndef GET_FILE_FROM_HOST
# define GET_FILE_FROM_HOST

# include <openssl/ssl.h>
# include <stdbool.h>
# define REQUEST_BUFFER_SIZE 1024

struct	host_data {
	char	*hostname;
	char	*filepath;
	char	*filename;
	int		is_secured;
};

struct	header_data {
	char	*status;
	char	*content_size;
	char	*redirect_url;
	char	*transfer_encoding;
	bool	is_html;
};

struct	dl_data {
	int				sock_fd;
	SSL				*ssl;
	FILE			*fp;
	unsigned long	bytes_per_sec;
	struct timespec	start_download_time;
	unsigned long	total_bytes_downloaded;
	bool			is_in_chunk;
	long			chunk_size;
	long 			chunk_data_count;
};

struct file_data {
	struct host_data	*host_data;
	struct header_data	*header_data;
	char				*file_path;
};

struct header_data	*download_file(int sock_fd, SSL *ssl,
					char *file_path, unsigned long bytes_per_sec, bool display);
char				*get_host_file_path(const char *storage_dir_path,
								char *file_name, struct host_data *host_data,
																int is_mirror);
struct host_data	*get_hostdata(char *url);
void				free_hostdata(struct host_data *host_data);

struct header_data	*skip_htpp_header(int sock_fd, SSL *ssl,
						char *response, int *received, int *remaining_data_len);
void				free_header_data(struct header_data *header_data);
#endif
