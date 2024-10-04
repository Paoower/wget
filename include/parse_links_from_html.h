#ifndef PARSE_LINKS_FROM_HTML
# define PARSE_LINKS_FROM_HTML

# include "tools.h"
# include "download_file_from_url.h"

void	register_attribute_link(char **cursor, arraystr *links, char **lines,
								char *reject_list, char *exclude_list,
								struct file_data *file_data,
								bool convert_links, bool is_mirror);
void	edit_link_in_buffer(char **lines,
										char *link_quote_pos, char *new_url);

#endif
