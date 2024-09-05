#include "tools.h"
#include <stdarg.h>

// Catch from src,
// The string between the first match of start and the first match of end
// Need to be freed after use.
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
char	*concat(const char *s, ...)
{
	va_list		args;
	char		*result;
	const char	*next;

	va_start(args, s);
	result = malloc(strlen(s) + 1);
	strcpy(result, s);
	while ((next = va_arg(args, const char *)) != NULL) {
		result = realloc(result, strlen(result) + strlen(next));
		strcat(result, next);
	}
	va_end(args);
	return result;
}
