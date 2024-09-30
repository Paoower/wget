#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "src.h"

#define MAX_URL_LENGTH 2048
#define MAX_URLS 1000

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

static char *extract_url(const char *tag, const char *attr) {
    char start_quote = '"';
    const char *start = strstr(tag, attr);
    if (!start) return NULL;
    start = strchr(start, '"');
    if (!start) start = strchr(tag, (start_quote = '\''));
    if (!start) return NULL;
    start++;
    const char *end = strchr(start, start_quote);
    if (!end) return NULL;
    size_t length = end - start;
    if (length >= MAX_URL_LENGTH) return NULL;
    char *url = malloc(length + 1);
    if (!url) return NULL;
    strncpy(url, start, length);
    url[length] = '\0';
    return url;
}

char **get_urls_from_html(char *file_path, char *reject_list, char *exclude_list) {
    FILE *file = fopen(file_path, "r");
    if (!file) return NULL;

    char **urls = malloc(sizeof(char*) * (MAX_URLS + 1));
    if (!urls) {
        fclose(file);
        return NULL;
    }

    int url_count = 0;
    char line[4096];

    while (fgets(line, sizeof(line), file) && url_count < MAX_URLS) {
        char *tag_positions[] = {strstr(line, "<a "), strstr(line, "<link "), strstr(line, "<img ")};
        const char *attributes[] = {"href=", "href=", "src="};

        for (int i = 0; i < 3; i++) {
            if (tag_positions[i]) {
                char *url = extract_url(tag_positions[i], attributes[i]);
                if (url && !is_url_in_list(url, reject_list) && !is_url_in_list(url, exclude_list)) {
                    // printf("attributes %s found at line %d with url \"%s\"\n",
                    // attributes[i], url_count, url);
                    urls[url_count++] = url;
                } else {
                    free(url);
                }
            }
        }
    }
    fclose(file);
    urls[url_count] = NULL;
    return urls;
}
