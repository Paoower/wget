#include "settings.h"
#include "src.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_URL_LENGTH 2048

static int is_url_in_list(const char *url, const char *list) {
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

// returns the starting quote
static char	move_cursor_at_quote(char **cursor)
{
	char	*new_cursor;

	if (!cursor || !*cursor)
		return 0;
	new_cursor = *cursor;
	new_cursor = strchr(*cursor, '=');
	if (!new_cursor)
		return 0;
	new_cursor++;
	while (*new_cursor && isspace((unsigned char)*new_cursor))
		new_cursor++;
	*cursor = new_cursor;
	if (*new_cursor == '"' || *new_cursor == '\'')
		return *new_cursor;
	return 0;
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
	if (does_match_with_pattern(link, FILE_PATH_REGEX)) { // not an url
		// has the right html attribut
		return true;
	}
	return false;
}

/**
 * move the cursor at the end of the parameter
 */
static char *get_url_from_param(char **cursor, char quote,
												struct file_data *file_data) {
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
	if (is_valid_host_link(url, file_data))
		return url;
	free(url);
	return NULL;
}

// lines may be edited
arraystr	extract_urls_from_line(char **lines, char *reject_list,
								char *exclude_list, struct file_data *file_data,
								bool convert_links, bool is_mirror)
{
	arraystr	urls;
	char		*cursor;
	char		*url;
	char		quote;

	(void)convert_links;
	(void)is_mirror;
	urls = NULL;
	cursor = *lines;
	while (*cursor && (cursor = strchr(cursor, '<'))) {
		while (*cursor && !isspace((unsigned char)*cursor))
				cursor++; // ignore beacon name
		while (*cursor && *cursor != '>') {
			while (*cursor && isspace((unsigned char)*cursor))
				cursor++; // ignore other spaces
			if (!*cursor || *cursor == '>')
				break;
			quote = move_cursor_at_quote(&cursor);
			if (!quote || !*cursor)
				break;
			url = get_url_from_param(&cursor, quote, file_data);
			if (url) {
				if (!is_url_in_list(url, reject_list)
									&& !is_url_in_list(url, exclude_list)) {
					// TODO: convert_link()
					// TODO: edit_link_in_buffer()
					arraystr_append(&urls, url);
					printf("Debug: Extracted URL: %s\n", url);  // Debug output
				}
				free(url);
			}
			cursor++;
		}
	}
	return urls;
}

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

void	edit_file_with_new_content(arraystr new_file_content)
{
	(void)new_file_content;
}

arraystr	parse_links_from_html(struct file_data *file_data,
										char *reject_list, char *exclude_list,
										bool convert_links, bool is_mirror) {
	FILE		*file;
	arraystr	urls,	new_urls;
	char		*line,	*lines;
	size_t		len,	lines_len;
	arraystr	new_file_content;

	file = fopen(file_data->file_path, "r");
	if (!file)
		return NULL;
	urls = NULL;
	line = NULL;
	lines = NULL;
	lines_len = 0;
	new_file_content = NULL;
	while (getline(&line, &len, file) != -1) {
		concat_buffer(line, len, &lines, &lines_len);
		if (does_lines_contains_full_beacon(lines)) {
			new_urls = extract_urls_from_line(&lines, reject_list,
							exclude_list, file_data, convert_links, is_mirror);
			if (new_urls) {
				arraystr_merge(&urls, new_urls);
				free(new_urls);
			}
			array_append((void ***)&new_file_content, (void *)lines);
			lines = NULL;
		}
		free(line);
		line = NULL;
	}
	free(line);
	fclose(file);
	edit_file_with_new_content(new_file_content);
	free_arraystr(new_file_content);
	return urls;
}
