#include "tools.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief
 * get any content of a line in the http response
 * @param spliter Optionnal string to split the line with (default ": ")
 * @return
 * return the string of the content which match with the key
 * The caller is responsible to free the result.
 */
char	*get_http_response_info(const char *http_response,
										const char *key, const char *spliter)
{
	char	**lines;
	char	**tokens;
	char	*result;
	int		i;

	if (!spliter)
		spliter = ": ";
	lines = split(http_response, "\r\n");
	i = 0;
	while (lines[i] != NULL) {
		tokens = split(lines[i], spliter);
		if (tokens[0] != NULL && strcmp(tokens[0], key) == 0) {
			result = strdup(lines[i] + strlen(tokens[0]) + strlen(spliter));
			free(tokens);
			return result;
		}
		free(tokens);
		i++;
	}
	free(lines);
	return NULL;
}
