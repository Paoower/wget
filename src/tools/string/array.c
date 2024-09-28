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
int	array_append(char ***heap_dest_array, char *src_str)
{
	int		i;
	int		array_length;
	char	**result;
	char	*new_str;

	if (!heap_dest_array || !src_str)
		return 1;
	new_str = strdup(src_str);
	if (!new_str)
		return 1;
	array_length = array_len(*heap_dest_array);
	result = malloc(sizeof(char *) * (array_length + 2));
	if (!result)
		return 1;
	i = 0;
	while (i < array_length) {
		result[i] = (*heap_dest_array)[i];
		i++;
	}
	result[i++] = new_str;
	result[i] = NULL;
	free(*heap_dest_array);
	*heap_dest_array = result;
	return 0;
}

/**
 * @brief Concatenate the two arrays over array1.
 * Keep every strings from array1 and copy every strings from array2
 * @param heap_array1 The first array. Must be dynamically created.
 * @param array2 The second array.
 * @return A new array that contains the two given arrays.
 * Use `free_array` to free memory after use.
 */
int	array_concat(char ***heap_dest_array, char **src_array)
{
	int		i;
	int		j;
	int		array1_length;
	int		array2_length;
	char	**result;
	char	*new_str;

	if (!heap_dest_array)
		return 1;
	array1_length = array_len(*heap_dest_array);
	array2_length = array_len(src_array);
	if (array2_length == 0)
		return 1;
	result = malloc(sizeof(char *) * (array1_length + array2_length + 1));
	if (!result)
		return 1;
	i = 0;
	j = 0;
	while (i < array1_length + array2_length) {
		if (i < array1_length)
			result[i] = (*heap_dest_array)[i];
		else {
			new_str = strdup(src_array[j]);
			if (!new_str) {
				while(j > 0) {
					free(result[i - j]);
					j--;
				}
				free(result);
				return 1;
			}
			result[i] = new_str;
			j++;
		}
		i++;
	}
	result[i] = NULL;
	free(*heap_dest_array);
	*heap_dest_array = result;
	return 0;
}
