#include "settings.h"
#include "parse_links_from_html.h"
#include "src.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char	*link_attributes[] = {
	"src",
	"href",
	"action",
	"formaction",
	"cite",
	"data",
	"usemap",
	"codebase",
	"manifest",
	"poster",
	"icon",
	"profile",
	"archive",
	"longdesc",
	"ping",
	"srcset",
	"integrity",
	"background",
	"srcdoc",
	"content",
	"url",
	NULL
};

static int is_link_in_list(const char *url, const char *list) {
	if (!list) return 0;
	char *list_copy = strdup(list);
	char *token = strtok(list_copy, ",");
	while (token) {
		if (strstr(url, token)) {
			free(list_copy);
			return 1;
		}
		token = strtok(NULL, ",");
	}
	free(list_copy);
	return 0;
}

char	find_next_attribut_quote(char **cursor)
{
	char	*ptr;
	char	*next_attr_quote;
	int		i;

	if (!cursor || !*cursor || !**cursor)
		return '\0';
	next_attr_quote = NULL;
	for (i = 0; link_attributes[i]; i++) {
		if ((ptr = strstr(*cursor, link_attributes[i])) && ptr) {
			ptr += strlen(link_attributes[i]);
			while (*ptr && isspace(*ptr))
				ptr++;
			if (*ptr != '=' && *ptr != '(')
				continue;
			ptr++;
			while (*ptr && isspace(*ptr))
				ptr++;
			if (*ptr == '\'' || *ptr == '"') {
				if (!next_attr_quote || ptr < next_attr_quote)
					next_attr_quote = ptr;
			}
		}
	}
	if (!next_attr_quote) {
		*cursor += strlen(*cursor); // set the cursor at the end
		return '\0';
	}
	*cursor = next_attr_quote;
	return *next_attr_quote;
}

static bool is_valid_host_link(char *link, struct file_data *file_data)
{
	char	*cursor;

	if (!file_data || !file_data->host_data)
		return false;
	if ((cursor = strstr(link, file_data->host_data->hostname))) {
		if (strchr(cursor, '/'))
		// if link is url with the right domain name and is followed by a '/'
			return true;
	}
	if (does_match_with_pattern(link, FILE_PATH_REGEX))
	// is not an url
		return true;
	return false;
}

/**
 * move the cursor at the end of the parameter
 */
static char *get_link(char **cursor, char quote, struct file_data *file_data) {
	char	*start;
	char	*end;
	char	*url;

	if (!cursor || !*cursor || !**cursor)
		return NULL;
	start = *cursor;
	start++;
	end = quote ? strchr(start, quote) : start;
	while (*end && *end != quote)
		end++;
	*cursor = end;
	size_t length = end - start;
	if (length >= MAX_URL_LENGTH)
		return NULL;
	url = malloc(length + 1);
	if (!url)
		return NULL;
	strncpy(url, start, length);
	url[length] = '\0';
	if (is_valid_host_link(url, file_data))
		return url;
	free(url);
	return NULL;
}

void	register_attribute_link(char **cursor, arraystr *links, char **lines,
								char *reject_list, char *exclude_list,
								struct file_data *file_data,
								bool convert_links, bool is_mirror)
{
	char	quote;
	char	*quote_pos;
	char	*link;
	char	*new_link;
	char	*online_link;

	quote = find_next_attribut_quote(cursor);
	if (!quote || !*cursor || !**cursor)
		return;
	quote_pos = *cursor;
	link = get_link(cursor, quote, file_data);
	if (link) {
		if (!is_link_in_list(link, reject_list)
							&& !is_link_in_list(link, exclude_list)) {
			if (convert_links) {
				new_link = convert_link(link, file_data, is_mirror);
				replace_link_in_buffer(cursor, lines, quote_pos, new_link);
				free(new_link);
			}
			if (is_mirror) {
				online_link = convert_link_to_online(link, file_data);
				if (online_link) {
					arraystr_append(links, online_link);
					free(online_link);
				}
			}
		}
		free(link);
	}
}
