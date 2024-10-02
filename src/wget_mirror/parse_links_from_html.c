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

static char *extract_url(const char *tag) {
	const char *start = strchr(tag, '=');
	if (!start) return NULL;
	start++;

	while (isspace(*start)) start++;

	char quote = (*start == '"' || *start == '\'') ? *start : 0;
	if (quote) start++;

	const char *end = quote ? strchr(start, quote) : start;
	while (*end && !isspace(*end) && *end != '>' && *end != quote) end++;

	size_t length = end - start;
	if (length >= MAX_URL_LENGTH) return NULL;

	char *url = malloc(length + 1);
	if (!url) return NULL;

	strncpy(url, start, length);
	url[length] = '\0';

	// Check if the URL starts with '/' or "http"
	if (url[0] == '/' || strncmp(url, "http", 4) == 0) {
		return url;
	}

	free(url);
	return NULL;
}

arraystr	parse_links_from_html(struct file_data *file_data,
										char *reject_list, char *exclude_list) {
	arraystr	urls;

	urls = arraystr_init(NULL);
	FILE *file = fopen(file_data->file_path, "r");
	if (!file) return NULL;

	int url_count = 0;
	char buffer[4096];
	size_t buffer_size = 0;

	// TODO: create a buffer that	contains the whole content of the file
	while (fgets(buffer + buffer_size, sizeof(buffer) - buffer_size, file)) {
		// url not spoted if the line is longer than the buffer
		buffer_size = strlen(buffer);
		char *tag_start = buffer;
		while ((tag_start = strchr(tag_start, '<'))) {
			char *tag_end = strchr(tag_start, '>');
			if (!tag_end) break;

			*tag_end = '\0';
			char *attr = tag_start;
			// TODO: be sure that the link is local or has the right domain name
			// domain name is stored in file_data->host_data->hostname
			while ((attr = strpbrk(attr, " \t\n\r\f\v")) != NULL) {
				attr++;
				char *url = extract_url(attr);
				// TODO: convert_link()
				// TODO: edit_link_in_buffer()
				if (url && !is_url_in_list(url, reject_list) && !is_url_in_list(url, exclude_list)) {
					arraystr_append(&urls, url);
					url_count++;
					printf("Debug: Extracted URL: %s\n", url);  // Debug output
				} else {
					free(url);
				}
			}

			*tag_end = '>';
			tag_start = tag_end + 1;
		}

		if (!feof(file) && !strchr(buffer, '\n')) {
			char *last_tag = strrchr(buffer, '<');
			if (last_tag) {
				buffer_size = strlen(last_tag);
				memmove(buffer, last_tag, buffer_size);
			} else {
				buffer_size = 0;
			}
		} else {
			buffer_size = 0;
		}
		// nani sore ?
		// TODO: concatenate the buffer into the buffer that contains all content
	}
	fclose(file);
	// TODO: edit the file with the new buffer that contains all the new content

	// Debug output
	printf("Debug: Total URLs extracted: %d\n", url_count);
	for (int i = 0; i < url_count; i++) {
		printf("Debug: URL %d: %s\n", i, urls[i]);
	}
	return urls;
}
