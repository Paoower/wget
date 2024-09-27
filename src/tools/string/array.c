#include "tools.h"
#include <stdlib.h>
#include <string.h>

void	free_array(char **heap_array)
{
	int	i;

	if (!heap_array)
		return;
	i = 0;
	while(heap_array[i])
		free(heap_array[i++]);
	free(heap_array);
}

int	array_len(char **array)
{
	int	count;

	if (!array)
		return 0;
	count = 0;
	while (array[count])
		count++;
	return count;
}

/**
 * @brief Append s to the given array.
 * @param heap_array The initial array. Must be dynamically created.
 * Put NULL to create an array.
 * @param str The string to add.
 * @return
 * The new array of string with s added,
 * or the initial array if something went wrong.
 * Use `free_array` to free memory after use.
 */
char	**array_append(char **heap_array, char *str)
{
	int		i;
	int		array_length;
	char	**result;
	char	*new_str;

	if (!str)
		return heap_array;
	new_str = strdup(str);
	if (!new_str)
		return heap_array;
	array_length = array_len(heap_array);
	result = malloc(sizeof(char *) * (array_length + 2));
	if (!result)
		return heap_array;
	i = 0;
	while (i < array_length) {
		result[i] = heap_array[i];
		i++;
	}
	result[i++] = new_str;
	result[i] = NULL;
	free(heap_array);
	return result;
}

/**
 * @brief Concatenate the two arrays over array1.
 * Keep every strings from array1 and copy every strings from array2
 * @param heap_array1 The first array. Must be dynamically created.
 * @param array2 The second array.
 * @return A new array that contains the two given arrays.
 * Use `free_array` to free memory after use.
 */
char	**array_concat(char **heap_array1, char **array2)
{
	int		i;
	int		j;
	int		array1_length;
	int		array2_length;
	char	**result;
	char	*new_str;

	array1_length = array_len(heap_array1);
	array2_length = array_len(array2);
	if (array2_length == 0)
		return heap_array1;
	result = malloc(sizeof(char *) * (array1_length + array2_length + 1));
	if (!result)
		return heap_array1;
	i = 0;
	j = 0;
	while (i < array1_length + array2_length) {
		if (i < array1_length)
			result[i] = heap_array1[i];
		else {
			new_str = strdup(array2[j]);
			if (!new_str) {
				while(j > 0) {
					free(result[i - j]);
					j--;
				}
				free(result);
				return heap_array1;
			}
			result[i] = new_str;
			j++;
		}
		i++;
	}
	result[i] = NULL;
	free(heap_array1);
	return result;
}
