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
	while (lines[i]) {
		tokens = split(lines[i], spliter);
		if (tokens) {
			if (tokens[0] && strcmp(tokens[0], key) == 0) {
				result = strdup(lines[i] + strlen(tokens[0]) + strlen(spliter));
				free_char_tab(tokens);
				free_char_tab(lines);
				return result;
			}
			free_char_tab(tokens);
		}
		i++;
	}
	free_char_tab(lines);
	return NULL;
}
