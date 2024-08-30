#include <string.h>

// catch from src,
// the string between the first match of start and the first match of end
char	*get_str_between(char *src, char *start, char *end)
{
	char	*dest;
	int		dest_len;

	if (!src || !start || !end)
		return NULL;
	start = strstr(src, start);
	if (!start)
		start = 0;
	end = strstr(src, end);
	if (!end)
		end = strlen(src) - 1;
	dest_len = end - start;
	dest = (char *)malloc(dest_len + 1);
	if (!dest) {
		perror("Memory allocation failed");
		return NULL;
	}
	strncpy(dest, src, dest_len);
	dest[dest_len] = '\0';
	return dest;
}
