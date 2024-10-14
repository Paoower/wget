#ifndef PARSE_LINKS_FROM_HTML
# define PARSE_LINKS_FROM_HTML

# include "tools.h"
# include "download_file_from_url.h"

# define MAX_URL_LENGTH 2048

char	find_next_attribut_quote(char **cursor);
void	register_attribute_link(char **cursor, arraystr *links, char **lines,
								char *reject_list, char *exclude_list,
								struct file_data *file_data,
								bool convert_links, bool is_mirror);
void	replace_link_in_buffer(char **cursor, char **lines,
										char *link_quote_pos, char *new_link);

#endif
