#include "src.h"
#include "tools.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

struct params_thread {
	struct parameters_t	params;
	char				*line;
};

char **read_lines_from_file(const char *filename, int *line_count) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "Erreur: Impossible d'ouvrir le fichier %s\n", filename);
		return NULL;
	}

	char **lines = NULL;
	char buffer[256];
	*line_count = 0;

	while (fgets(buffer, sizeof(buffer), file)) {
		buffer[strcspn(buffer, "\n")] = '\0';

		lines = realloc(lines, sizeof(char *) * (*line_count + 1));
		if (!lines) {
			fprintf(stderr, "Erreur: Problème d'allocation mémoire\n");
			fclose(file);
			return NULL;
		}

		lines[*line_count] = strdup(buffer);
		if (!lines[*line_count]) {
			fprintf(stderr, "Erreur: Problème d'allocation mémoire pour la ligne\n");
			fclose(file);
			return NULL;
		}

		(*line_count)++;
	}

	fclose(file);
	return lines;
}

void free_lines(char **lines, int line_count)
{
	for (int i = 0; i < line_count; i++) {
		free(lines[i]);
	}
	free(lines);
}

void apply_function_to_line(char *line, struct parameters_t params)
{

	char	*file_path;
	printf("Traitement de la ligne: %s\n", line);
	file_path = download_file_from_url(line, params.storage_path,
						params.output_file, params.rate_limit, params.mirror);
	if (!file_path)
		return ;
	printf("Downloaded [%s]\n", file_path);
	free(file_path);
	print_current_date("finished at ");

	return ;
}

void *thread_function(void *arg)
{
	struct params_thread	*params_thread;

	params_thread = (struct params_thread *)arg;
	printf("Wesh %s\n", params_thread->line);
	apply_function_to_line(params_thread->line, params_thread->params);
	free(params_thread->line);
	return NULL;
}

int wget_from_file(struct parameters_t parameters)
{
	const char *filename = parameters.links_file;
	int line_count = 0;

	char **lines = read_lines_from_file(filename, &line_count);
	if (!lines) {
		return 1;
	}

	pthread_t *threads = malloc(line_count * sizeof(pthread_t));
	for (int i = 0; i < line_count; i++) {
		struct params_thread params_thread;
		params_thread.line = lines[i];
		params_thread.params = parameters;
		printf("Print de test, c'est le thread numero %d \n", i);
		printf("%s\n", params_thread.line);
		int rc = pthread_create(&threads[i], NULL, thread_function, (void *)&params_thread);
		if (rc) {
			fprintf(stderr, "Erreur lors de la création du thread, code : %d\n", rc);
			exit(EXIT_FAILURE);
		}
	}
	for (int i = 0; i < line_count; i++) {
		pthread_join(threads[i], NULL);
	}
	free_lines(lines, line_count);
	free(threads);

	printf("Tous les traitements sont terminés.\n");
	return 0;
}
