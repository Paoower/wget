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

array_str	get_urls_from_html(struct file_data *file_data,
										char *reject_list, char *exclude_list) {
	array_str	urls;

	urls = array_str_init(NULL);
	FILE *file = fopen(file_data->file_path, "r");
	if (!file) return NULL;

	int url_count = 0;
	char buffer[4096];
	size_t buffer_size = 0;

	while (fgets(buffer + buffer_size, sizeof(buffer) - buffer_size, file)) {
		buffer_size = strlen(buffer);
		char *tag_start = buffer;

		while ((tag_start = strchr(tag_start, '<'))) {
			char *tag_end = strchr(tag_start, '>');
			if (!tag_end) break;

			*tag_end = '\0';
			char *attr = tag_start;

			while ((attr = strpbrk(attr, " \t\n\r\f\v")) != NULL) {
				attr++;
				char *url = extract_url(attr);
				if (url && !is_url_in_list(url, reject_list) && !is_url_in_list(url, exclude_list)) {
					array_str_append(&urls, url);
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
	}
	fclose(file);

	// Debug output
	printf("Debug: Total URLs extracted: %d\n", url_count);
	for (int i = 0; i < url_count; i++) {
		printf("Debug: URL %d: %s\n", i, urls[i]);
	}
	return urls;
}
