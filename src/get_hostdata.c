#include "srcs.h"
#include "tools.h"


void	free_hostdata(struct host_data *host_data)
{
	if (!host_data)
		return;
	free(host_data->hostname);
	free(host_data->filename);
	free(host_data->filepath);
	free(host_data);
}

struct host_data	*get_hostdata(char *url, char *file_name)
{
	struct host_data	*host_data;

	if (!does_match_with_pattern(url, URL_REGEX)) {
		fprintf(stderr, "Error : not an url (%s)\n", url);
		return NULL;
	}
	host_data = malloc(sizeof(struct host_data));
	if (!host_data) {
		perror("Memory allocation failed");
		return NULL;
	}
	if (file_name)
		host_data->filename = file_name;
	else
		host_data->filename = strdup("interestellar-cry.mp4");
	// NEED CODE TO FILL HOSTNAME, FILENAME AND FILEPATH
	// temporary test
	host_data->hostname = strdup("images-ext-1.discordapp.net");
	host_data->filepath = strdup("/external/0Z9r33Q7sYmy6pq97wKmO0BkzYm9S-9DbN72Ycu4mt4/https/media.tenor.com/xueKI4wn14YAAAPo/interestellar-cry.mp4");
	if (!host_data->hostname ||
			!host_data->filename || !host_data->hostname) {
		free_hostdata(host_data);
		return NULL;
	}
	return host_data;
}
