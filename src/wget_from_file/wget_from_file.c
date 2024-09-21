#include "src.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

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

void apply_function_to_line(const char *line)
{
	printf("Traitement de la ligne: %s\n", line);
	// get_file_from_host(line);
}

void *thread_function(void *line)
{
	char *line_content = (char *)line;

	apply_function_to_line(line_content);

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
		pthread_create(&threads[i], NULL, thread_function, (void *)lines[i]);
	}

	for (int i = 0; i < line_count; i++) {
		pthread_join(threads[i], NULL);
	}

	free_lines(lines, line_count);
	free(threads);

	printf("Tous les traitements sont terminés.\n");

	return 0;
	printf("%d\n", parameters.mirror);
	return 0;
}
