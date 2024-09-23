#ifndef SRC_H
# define SRC_H

# include <time.h>

struct parameters_t {
	char			*url;
	char			*url_filename;
	char			*url_hostname;
	char			*url_filepath;
	int				background;
	char			*output_file;
	char			*storage_path;
	unsigned long	rate_limit;
	char			*links_file;
	int				mirror;
	char			*reject_list;
	char			*exclude_list;
	int				convert_links;
};

int		handle_args(struct parameters_t *parameters, int argc, char *argv[]);
void	free_args(struct parameters_t *params);
char	*download_file_from_url(char *url, char *storage_dir_path,
				char *file_name, unsigned long bytes_per_sec, int is_mirror);

int		wget(struct parameters_t params);
int		wget_in_background(struct parameters_t params);
// wget

int		wget_from_file(struct parameters_t params);
int		wget_mirror(char *url, struct parameters_t params);
// wget mode

#endif
