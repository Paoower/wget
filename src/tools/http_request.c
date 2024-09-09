#include <time.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief
 * get any content of a line in the http response
 * @param spliter Optionnal string to split the line with (default ':')
 * @return
 * return the string of the content which match with the key
 * The caller is responsible to free the result.
 */
char	*get_http_response_info(const char *http_response,
										const char *key, const char *spliter)
{
	char	*response_cpy;
	char	*line;
	char	*line_cpy;
	char	*line_token;
	char	*result;

	if (!spliter)
		spliter = ": ";
	response_cpy = strdup(http_response);
	if (!response_cpy)
		return NULL;
	line = strtok(response_cpy, "\r\n");
	while (line != NULL) {
		line_cpy = strdup(line);
		if (!line_cpy)
			continue;
		line_token = strtok(line_cpy, spliter);
		if (line_token != NULL && strcmp(line_token, key) == 0) {
			result = strdup(line_cpy + strlen(line_token) + strlen(spliter));
			free(line_cpy);
			return result;
		}
		free(line_cpy);
		line = strtok(NULL, "\r\n");
	}
	free(response_cpy);
	return NULL;
}
