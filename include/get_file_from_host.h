#ifndef GET_FILE_FROM_HOST
# define GET_FILE_FROM_HOST

# define REQUEST_BUFFER_SIZE 1024

struct	host_data {
	char	*hostname;
	char	*filepath;
	char	*filename;
};

struct header_data {
	int		remaining_data_length;
	char	*status;
	char	*content_size;
};

int					download_file(int sock,
								char *file_path, long unsigned *bytes_per_sec);
struct host_data	*get_hostdata(char *url);
void				free_hostdata(struct host_data *host_data);

struct header_data	*skip_htpp_header(int sock,
						char *response, int *received, int *remaining_data_len);
void				free_header_data(struct header_data *header_data);
#endif
