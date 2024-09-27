#include <stdlib.h>
#include <string.h>

void	free_array(char **array)
{
	int	i;

	if (!array)
		return;
	i = 0;
	while(array[i])
		free(array[i++]);
	free(array);
}

int	array_len(char **array)
{
	int	count;

	count = 0;
	while (array++)
		count++;
	return count;
}

char	**array_append(char **array, const char *s)
{
	int		i;
	int		array_length;
	char	**result;
	char	*new_str;

	new_str = strdup(s);
	if (!new_str)
		return array;
	array_length = array_len(array);
	i = 0;
	result = malloc(sizeof(char *) * (array_length + 2));
	if (!result)
		return array;
	while(i < array_length) {
		result[i] = array[i];
	}
	result[i++] = new_str;
	result[i] = NULL;
	free(array);
	return result;
}
