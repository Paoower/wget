#ifndef GET_FILE_FROM_HOST
# define GET_FILE_FROM_HOST

# include <time.h>

# define REQUEST_BUFFER_SIZE 1024

struct	host_data {
	char	*hostname;
	char	*filepath;
	char	*filename;
};

int					download_file(int sock, char *dir_path,
							char *file_name, long unsigned int *bytes_per_sec);
struct host_data	*get_hostdata(char *url);
void				free_hostdata(struct host_data *host_data);


void				print_final_download_infos(struct timespec elapsed_time,
									long unsigned int total_bytes_downloaded);
void				print_current_date(char *text_before);
// print

#endif
