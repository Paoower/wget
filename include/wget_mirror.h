#ifndef WGET_MIRROR_H
#define WGET_MIRROR_H

#include "src.h"

int wget_mirror(char *url, struct parameters_t params);
int 	convert_links_for_offline_viewing(char *file_path);
char	**get_urls_from_html(char	*file_path,
										char *reject_list, char *exclude_list);

#endif
