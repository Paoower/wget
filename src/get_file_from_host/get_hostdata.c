#include "get_file_from_host.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_hostdata(struct host_data *host_data)
{
    if (!host_data)
        return;
    free(host_data->hostname);
    free(host_data->filename);
    free(host_data->filepath);
    free(host_data);
}

struct host_data *get_hostdata(char *url)
{
    struct host_data *host_data;
    char *protocol_end, *hostname_end, *filepath_start;

    host_data = malloc(sizeof(struct host_data));
    if (!host_data) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Check if the URL is secured (HTTPS)
    if (strncmp(url, "https://", 8) == 0) {
        host_data->is_secured = 1;
        protocol_end = url + 8;
    } else if (strncmp(url, "http://", 7) == 0) {
        host_data->is_secured = 0;
        protocol_end = url + 7;
    } else {
        // Default to HTTP if no protocol is specified
        host_data->is_secured = 0;
        protocol_end = url;
    }

    // Find end of hostname
    hostname_end = strchr(protocol_end, '/');
    if (!hostname_end) {
        // URL doesn't contain a path, just a hostname
        host_data->hostname = strdup(protocol_end);
        host_data->filepath = strdup("/");
    } else {
        // Extract hostname
        host_data->hostname = strndup(protocol_end, hostname_end - protocol_end);

        // Extract filepath
        filepath_start = hostname_end;
        host_data->filepath = strdup(filepath_start);
    }

    // Extract filename (last part of the filepath)
    char *filename_start = strrchr(host_data->filepath, '/');
    if (filename_start && *(filename_start + 1) != '\0') {
        host_data->filename = strdup(filename_start + 1);
    } else {
        // If no filename is found, use a default name
        host_data->filename = strdup("index.html");
    }

    // Check if memory allocation was successful
    if (!host_data->hostname || !host_data->filepath || !host_data->filename) {
        perror("Memory allocation failed");
        free_hostdata(host_data);
        return NULL;
    }

    return host_data;
}
