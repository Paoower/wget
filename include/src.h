#ifndef SRC_H
# define SRC_H

# include <time.h>

struct parameters_t {
	char	*url;
	char	*url_filename;
	char	*url_hostname;
	char	*url_filepath;
	int		background;
	char	*output_file;
	char	*storage_path;
	int		rate_limit;
	char	*links_file;
	int		mirror;
	char	*reject_list;
	char	*exclude_list;
	int		convert_links;
};

int		handle_args(struct parameters_t *parameters, int argc, char *argv[]);
void	free_args(struct parameters_t *params);
int		get_file_from_host(char *url, char *storage_dir_path,
							char *file_name, long unsigned *bytes_per_sec);

int		wget(struct parameters_t params);
int		wget_in_background(struct parameters_t params);
// wget

#endif
