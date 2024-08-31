#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// catch from src,
// the string between the first match of start and the first match of end
// need to be freed after use.
char	*get_str_between(char *src, char *start, char *end)
{
	char	*dest;
	int		dest_len;
	char	*start_pos;
	char	*end_pos;

	if (!src || !start || !end)
		return NULL;
	start_pos = strstr(src, start);
	if (!start_pos)
		start_pos = src;
	end_pos = strstr(src, end);
	if (!end_pos)
		end_pos = src + strlen(src) - 1;
	dest_len = end_pos - start_pos;
	dest = (char *)malloc(dest_len + 1);
	if (!dest) {
		perror("Memory allocation failed");
		return NULL;
	}
	strncpy(dest, src, dest_len);
	dest[dest_len] = '\0';
	return dest;
}

// need to be freed at the end
char	*concat(char *s1, char *s2)
{
	char	*result;

	result = malloc(strlen(s1) + strlen(s2) + 1);
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}
