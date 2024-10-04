#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/**
 * Catch from src the string between the first match of start
 * and the first match of end
 * @return
 * Pointer to a copy of the string between start and end of src.
 * The caller is responsible for freeing this memory.
 */
char *get_str_between(char *src, char *start, char *end)
{
	char *dest;
	int dest_len;
	char *start_pos;
	char *end_pos;

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
	if (!dest)
	{
		perror("Memory allocation failed");
		return NULL;
	}
	strncpy(dest, src, dest_len);
	dest[dest_len] = '\0';
	return dest;
}

char *str_concat(const char *s, ...)
{
	va_list args;
	const char *next;
	char *result;
	char *temp;

	va_start(args, s);
	result = strdup(s);
	if (!result)
	{
		perror("Failed to duplicate initial string");
		va_end(args);
		return NULL;
	}
	while ((next = va_arg(args, const char *)) != NULL)
	{
		temp = realloc(result, strlen(result) + strlen(next) + 1);
		if (!temp)
		{
			perror("Failed to reallocate memory");
			free(result);
			va_end(args);
			return NULL;
		}
		result = temp;
		strcat(result, next);
	}
	va_end(args);
	return result;
}

void copy_string(char **dest, char *src)
{
	*dest = malloc(strlen(src) + 1);
	if (*dest != NULL)
	{
		strcpy(*dest, src);
	}
}

char	*trim_spaces(char *str)
{
	int		i;
	int		j;
	int		count_spaces;
	char	*result;

	i = 0;
	count_spaces = 0;
	while (str[i]) {
		if (isspace((unsigned int)str[i++]))
			count_spaces++;
	}
	if (count_spaces == 0)
		return NULL;
	result = malloc(strlen(str) - count_spaces + 1);
	if (!result)
		return NULL;
	i = 0;
	j = 0;
	while (str[i]) {
		if (!isspace((unsigned int)str[i]))
			result[j++] = str[i];
		i++;
	}
	result[j] = '\0';
	return result;
}
