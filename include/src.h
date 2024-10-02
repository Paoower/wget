#ifndef SRC_H
# define SRC_H

# include "tools.h"
# include "download_file_from_url.h"
# include <time.h>
# include <stdbool.h>

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

int					handle_args(struct parameters_t *parameters,
														int argc, char *argv[]);
void				free_args(struct parameters_t *params);
void				free_file_data(struct file_data *file_data);
struct file_data	*download_file_from_url(char *url, char *storage_dir_path,
								char *file_name, unsigned long bytes_per_sec,
								int is_mirror, bool display);

int					wget(struct parameters_t params);
int					wget_in_background(struct parameters_t params);
// wget

int					wget_from_file(struct parameters_t params);
int					wget_mirror(char *url, struct parameters_t params);
// wget mode

array_str			get_urls_from_html(struct file_data *file_data,
										char *reject_list, char *exclude_list);

char				*convert_link(char *link,
								struct file_data *file_data, bool is_mirror);

#endif
