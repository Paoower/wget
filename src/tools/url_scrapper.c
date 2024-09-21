// #include "src.h"
// #include "download_file_from_url.h"
// #include "tools.h"
// #include <stdio.h>
// #include <netdb.h>
// #include <pthread.h>
// #include <stdlib.h>
// #include <string.h>

// char **read_urls_from_file(const char *filename, int *url_count) {
// 	FILE *file = fopen(filename, "r");
// 	if (!file) {
// 		fprintf(stderr, "Erreur: Impossible d'ouvrir le fichier %s\n", filename);
// 		return NULL;
// 	}

// 	char **urls = NULL;
// 	char line[256];
// 	*url_count = 0;

// 	// Lire chaque ligne du fichier
// 	while (fgets(line, sizeof(line), file)) {
// 		line[strcspn(line, "\n")] = 0;  // Enlever le saut de ligne
// 		urls = realloc(urls, sizeof(char *) * (*url_count + 1));
// 		urls[*url_count] = strdup(line);  // Ajouter l'URL lue
// 		(*url_count)++;
// 	}

// 	fclose(file);
// 	return urls;
// }


// // Fonction exécutée par chaque thread pour télécharger un fichier
// void *thread_download(void *url) {
// 	char *file_url = (char *)url;

// 	// Appel de votre fonction de téléchargement
// 	download_file_without_header(file_url);

// 	// Extraire le nom du fichier pour afficher un message
// 	char *filename = strrchr(file_url, '/') + 1;
// 	printf("Download done for: %s\n", filename);

// 	return NULL;
// }