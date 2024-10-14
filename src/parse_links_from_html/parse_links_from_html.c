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

bool	does_lines_contains_full_links(char *lines)
{
	char	*cursor;
	char	quote;

	cursor = lines;
	while ((quote = find_next_attribut_quote(&cursor)) && cursor && *cursor) {
		cursor++;
		if (!(cursor = strchr(cursor, quote)))
			return false;
	}
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
	while (*cursor) {
		register_attribute_link(&cursor, &links, lines, reject_list,
							exclude_list, file_data, convert_links, is_mirror);
		if (*cursor)
			cursor++;
	}
	return links;
}

/**
 * @brief Write the concatanation of every buffers that contains
 * `new_file_content` over the file
 */
void	edit_file_with_new_content(char *file_path, arraystr new_content_arr)
{
	FILE	*file;
	char	*new_content;

	if (!file_path || !new_content_arr)
		return;
	file = fopen(file_path, "w");
	if (!file)
		return;
	new_content = arraystr_join(new_content_arr);
	if (!new_content)
		return;
	fprintf(file, "%s", new_content);
	free(new_content);
	fclose(file);
}

/**
 * @brief Parse the html given in the file_data and catch every links
 * for download or edit purpose.
 * @param file_data Objet that contains necessary variables like file_path and
 * host_data with hostname set.
 * @param reject_list List of file suffixes that the program will
 * avoid downloading during the retrieval.
 * @param exclude_list List of paths that the program
 * will avoid to follow and retrieve.
 * @param convert_links On true, convert links in html to online.
 * If mirror is also true, convert links in html to offline.
 * @param is_mirror On true, returns an array that contains every links
 * that are related to the host.
 * @return The array of the links turned into urls if is_mirror is true.
 * Else returns NULL
 */
arraystr	parse_links_from_html(struct file_data *file_data,
										char *reject_list, char *exclude_list,
										bool convert_links, bool is_mirror)
{
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
		if (does_lines_contains_full_links(lines)) {
			new_links = catch_links_from_lines(&lines, reject_list,
							exclude_list, file_data, convert_links, is_mirror);
			if (new_links) {
				arraystr_merge(&links, new_links);
				free_arraystr(new_links);
			}
			array_append((void ***)&new_file_content, (void *)lines);
			lines = NULL;
		}
		free(line);
		line = NULL;
	}
	free(line);
	fclose(file);
	if (convert_links)
		edit_file_with_new_content(file_data->file_path, new_file_content);
	free_arraystr(new_file_content);
	return links;
}
