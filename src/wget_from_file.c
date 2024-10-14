#include "src.h"
#include "tools.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

struct params_thread {
	struct parameters_t	params;
	char				*line;
	SSL_CTX				*ctx;
};

char **read_lines_from_file(const char *filename, int *line_count) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "Error: can't open the file %s\n", filename);
		return NULL;
	}

	char **lines = NULL;
	char buffer[256];
	*line_count = 0;

	while (fgets(buffer, sizeof(buffer), file)) {
		buffer[strcspn(buffer, "\n")] = '\0';

		lines = realloc(lines, sizeof(char *) * (*line_count + 1));
		if (!lines) {
			fprintf(stderr, "Error : problem while allocating memory\n");
			fclose(file);
			return NULL;
		}

		lines[*line_count] = strdup(buffer);
		if (!lines[*line_count]) {
			fprintf(stderr, "Error : problem while allocating memory for the line\n");
			fclose(file);
			return NULL;
		}

		(*line_count)++;
	}

	fclose(file);
	return lines;
}

void wget_from_file_cleanup(struct params_thread **params_threads,
											char **lines, int line_count)
{
	for (int i = 0; i < line_count; i++) {
		free(lines[i]);
		free(params_threads[i]);
	}
	free(lines);
	free(params_threads);
}

void apply_function_to_line(SSL_CTX *ctx, char *line, struct parameters_t params)
{
	struct file_data	*file_data;

	file_data = download_file_from_url(ctx, line, params.storage_path,
									params.output_file, params.rate_limit,
									params.mirror, false, params.background);
	if (!file_data || !file_data->file_path) {
		free_file_data(file_data);
		return;
	}
	printf("Downloaded [%s]\n", file_data->file_path);
	free_file_data(file_data);
	print_current_date("finished at ");

	return;
}

void *thread_function(void *arg)
{
	struct params_thread	*params_thread;

	params_thread = (struct params_thread *)arg;
	apply_function_to_line(params_thread->ctx,
									params_thread->line, params_thread->params);
	return NULL;
}

int wget_from_file(SSL_CTX *ctx, struct parameters_t parameters)
{
	struct params_thread	**params_threads;
	const char *filename = parameters.links_file;
	int line_count = 0;

	char **lines = read_lines_from_file(filename, &line_count);
	if (!lines) {
		return 1;
	}
	params_threads = malloc(sizeof(struct params_thread) * line_count);
	pthread_t *threads = malloc(line_count * sizeof(pthread_t));
	for (int i = 0; i < line_count; i++) {
		params_threads[i] = malloc(sizeof(struct params_thread));
		params_threads[i]->line = lines[i];
		params_threads[i]->params = parameters;
		params_threads[i]->ctx = ctx;
		int rc = pthread_create(&threads[i], NULL,
									thread_function, (void *)params_threads[i]);
		if (rc) {
			fprintf(stderr, "Error during the initialization of the thread, code : %d\n", rc);
			exit(EXIT_FAILURE);
		}
	}
	for (int i = 0; i < line_count; i++) {
		pthread_join(threads[i], NULL);
	}
	wget_from_file_cleanup(params_threads, lines, line_count);
	free(threads);
	printf("All Downloads completed.\n");
	return 0;
}
