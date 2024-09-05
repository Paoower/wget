#ifndef SRCS_H
# define SRCS_H

# define REQUEST_BUFFER_SIZE 1024

struct	host_data {
	char	*hostname;
	char	*filepath;
	char	*filename;
};

struct parameters_t
{
	char *url;
	char *url_filename;
	char *url_hostname;
	char *url_filepath;
	int background;
	char *output_file;
	char *file_path;
	int rate_limit;
	char *links_file;
	int mirror;
	char *reject_list;
	char *exclude_list;
	int convert_links;
};

extern struct parameters_t params;

// Download file
int					download_file(char *url, char *storage_dir_path, char *file_name);
struct host_data	*get_hostdata(char *url);
void				free_hostdata(struct host_data *host_data);

// Arguments handling
int handle_args(int argc, char *argv[]);

#endif
