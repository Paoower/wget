#include "parse_links_from_html.h"
#include "src.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void	concat_buffer(char *line, size_t len, char **lines, size_t *lines_len)
{
	char	*temp;

	if (!lines)
		return;
	if (!*lines) {
		*lines = strdup(line);
		return;
	}
	*lines_len += len;
	temp = str_concat(*lines, line, NULL);
	free(*lines);
	*lines = temp;
}

bool	does_lines_contains_full_beacon(char *lines)
{
	while ((lines = strchr(lines, '<')))
		if (!(lines = strchr(lines, '>')))
			return false;
	return true;
}

// lines may be edited
arraystr	catch_links_from_lines(char **lines, char *reject_list,
								char *exclude_list, struct file_data *file_data,
								bool convert_links, bool is_mirror)
{
	arraystr	links;
	char		*cursor;

	links = NULL;
	cursor = *lines;
	while (*cursor && (cursor = strchr(cursor, '<'))) {
		while (*cursor && !isspace((unsigned char)*cursor))
				cursor++; // ignore beacon name
		while (*cursor && *cursor != '>') {
			while (*cursor && isspace((unsigned char)*cursor))
				cursor++; // ignore other spaces
			if (!*cursor || *cursor == '>')
				break;
			register_attribute_link(&cursor, &links, lines, reject_list,
							exclude_list, file_data, convert_links, is_mirror);
			cursor++;
		}
	}
	return links;
}

/**
 * @brief Write the concatanation of every buffers that contains
 * `new_file_content` over the file
 */
void	edit_file_with_new_content(char *file_path, arraystr new_file_content)
{
	(void)new_file_content;
}

arraystr	parse_links_from_html(struct file_data *file_data,
										char *reject_list, char *exclude_list,
										bool convert_links, bool is_mirror) {
	FILE		*file;
	arraystr	links,	new_links;
	char		*line,	*lines;
	size_t		len,	lines_len;
	arraystr	new_file_content;

	file = fopen(file_data->file_path, "r");
	if (!file)
		return NULL;
	links = NULL;
	line = NULL;
	lines = NULL;
	lines_len = 0;
	new_file_content = NULL;
	while (getline(&line, &len, file) != -1) {
		concat_buffer(line, len, &lines, &lines_len);
		if (does_lines_contains_full_beacon(lines)) {
			new_links = catch_links_from_lines(&lines, reject_list,
							exclude_list, file_data, convert_links, is_mirror);
			if (new_links) {
				arraystr_merge(&links, new_links);
				free(new_links);
			}
			array_append((void ***)&new_file_content, (void *)lines);
			lines = NULL;
		}
		free(line);
		line = NULL;
	}
	free(line);
	fclose(file);
	edit_file_with_new_content(file_data->file_path, new_file_content);
	free_arraystr(new_file_content);
	return links;
}
