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

/**
 * @brief Get the attribute and move the cursor after it.
 * @return The attribute
 */
static char	*get_attribute(char **cursor)
{
	char	*end_attribut;
	char	*attribute;
	char	*temp;

	if (!cursor || !*cursor)
		return NULL;
	end_attribut = strchr(*cursor, '=');
	if (!end_attribut)
		return NULL;
	attribute = strndup(*cursor, end_attribut - *cursor);
	if (!attribute)
		return NULL;
	temp = trim_spaces(attribute);
	if (temp) {
		free(attribute);
		attribute = temp;
	}
	*cursor = end_attribut;
	return attribute;
}

static char get_quote(char **cursor)
{
	if (!cursor || !*cursor)
		return 0;
	(*cursor)++;
	while (**cursor && isspace((unsigned char)**cursor))
		(*cursor)++;
	if (**cursor == '"' || **cursor == '\'')
		return **cursor;
	return 0;
}

static bool is_valid_host_link(char *link,
								struct file_data *file_data, char *attribute)
{
	char	*cursor;

	if (!file_data || !file_data->host_data)
		return false;
	if ((cursor = strstr(link, file_data->host_data->hostname))) {
		if (strchr(cursor, '/'))
		// if link is url with the right domain name and is followed by a '/'
			return true;
	}
	if (does_match_with_pattern(link, FILE_PATH_REGEX)
						&& is_in_arraystr((arraystr)link_attributes, attribute))
	// is not an url and has the right html attribut
		return true;
	return false;
}

/**
 * move the cursor at the end of the parameter
 */
static char *get_link(char **cursor, char quote,
								struct file_data *file_data, char *attribute) {
	char	*start;
	char	*end;
	char	*url;

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
	if (is_valid_host_link(url, file_data, attribute))
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
	char	*attribute;
	char	*quote_pos;
	char	*link;
	int		offset;

	attribute = get_attribute(cursor);
	quote = get_quote(cursor);
	quote_pos = *cursor;
	link = get_link(cursor, quote, file_data, attribute);
	if (link) {
		if (!is_link_in_list(link, reject_list)
							&& !is_link_in_list(link, exclude_list)) {
			convert_link(&link, file_data, convert_links, is_mirror);
			offset = replace_link_in_buffer(lines, quote_pos, link);
			*cursor += offset;
			arraystr_append(links, link);
		}
		free(link);
	}
	if (attribute)
		free(attribute);
}
