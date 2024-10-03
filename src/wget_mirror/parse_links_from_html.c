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

/**
 * move the cursor at the end of the parameter
 */
static char *get_url_from_param(char **tag) {
	if (!tag)
		return NULL;
	char *start = strchr(*tag, '=');
	if (!start) return NULL;
	start++;

	while (isspace(*start)) start++;

	char quote = (*start == '"' || *start == '\'') ? *start : 0;
	if (quote) start++;

	char *end = quote ? strchr(start, quote) : start;
	while (*end && *end != quote)
		end++;
	*tag = end;
	size_t length = end - start;
	if (length >= MAX_URL_LENGTH) return NULL;

	char *url = malloc(length + 1);
	if (!url) return NULL;

	strncpy(url, start, length);
	url[length] = '\0';

	// Check if the URL starts with '/' or "http"
	// TODO: be sure that the link is local or has the right domain name
	// domain name is stored in file_data->host_data->hostname
	if (url[0] == '/' || strncmp(url, "http", 4) == 0) {
		return url;
	}

	free(url);
	return NULL;
}

// lines may be edited
arraystr	extract_urls_from_line(char **lines, char *reject_list,
						char *exclude_list, bool convert_links, bool is_mirror)
{
	arraystr	urls;
	char		*cursor;
	char		*url;

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
			url = get_url_from_param(&cursor);
			if (url) {
				if (!is_url_in_list(url, reject_list)
									&& !is_url_in_list(url, exclude_list)) {
					// TODO: convert_link()
					// TODO: edit_link_in_buffer()
					arraystr_append(&urls, url);
					printf("Debug: Extracted URL: %s\n", url);  // Debug output
					free(url);
				}
			}
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
			// new_urls = extract_urls_from_line(&lines, reject_list,
			// 							exclude_list, convert_links, is_mirror);
			// arraystr_merge(&urls, new_urls);
			// free(new_urls);
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
