#include "tools.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void	free_array(array array)
{
	int	i;

	if (!array)
		return;
	i = 0;
	while(array[i])
		free(array[i++]);
	free(array);
}

/**
 * @brief Free the array and set the pointer to NULL.
 * @param array Pointer to an array that has been created dynamically.
 */
void	clean_array(array *array)
{
	free_array(*array);
	*array = NULL;
}

int	array_len(array array)
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
 * @brief Init array with x values.
 * @param str The list of strings to add. Must be terminated by a NULL.
 * @return
 * Pointer to an array made of the given strings or NULL if the creation fails.
 * Use `free_array` or `clean_array` to free memory after use.
 */
array	array_init(const char *str, ...)
{
	va_list		args;
	int			size;
	array		result;
	int			i;

	va_start(args, str);
	size = arg_ptr_len(args, (void *)str);
	va_end(args);
	if (size == 0)
		return NULL;
	va_start(args, str);
	result = malloc(sizeof(char *) * (size + 1));
	i = 0;
	while (str) {
		if (i == size) {
			clean_array(&result);
			return NULL;
		}
		result[i] = strdup(str);
		if (!result[i]) {
			clean_array(&result);
			return NULL;
		}
		i++;
		str = va_arg(args, const char *);
	}
	result[i] = NULL;
	va_end(args);
	return result;
}

/**
 * @brief Append s to the given array.
 * @param heap_array The initial array. Must be dynamically created.
 * Put NULL to create an array.
 * @param str The string to add.
 * @return
 * The new array of string with s added,
 * or the initial array if something went wrong.
 * Use `free_array` or `clean_array` to free memory after use.
 */
int	array_append(array *dest, char *src)
{
	int		i;
	int		array_length;
	array	result;
	char	*new_str;

	if (!dest || !src)
		return 1;
	new_str = strdup(src);
	if (!new_str)
		return 1;
	array_length = array_len(*dest);
	result = malloc(sizeof(char *) * (array_length + 2));
	if (!result)
		return 1;
	i = 0;
	while (i < array_length) {
		result[i] = (*dest)[i];
		i++;
	}
	result[i++] = new_str;
	result[i] = NULL;
	free(*dest);
	*dest = result;
	return 0;
}

/**
 * @brief Merge the two arrays over array1.
 * Keep every strings from array1 and copy every strings from array2
 * @param heap_array1 The first array. Must be dynamically created.
 * @param array2 The second array.
 * @return A new array that contains the two given arrays.
 * Use `free_array` or `clean_array` to free memory after use.
 */
int	array_merge(array *dest, array src)
{
	int		i;
	int		j;
	int		array1_length;
	int		array2_length;
	char	**result;
	char	*new_str;

	if (!dest)
		return 1;
	array1_length = array_len(*dest);
	array2_length = array_len(src);
	if (array2_length == 0)
		return 1;
	result = malloc(sizeof(char *) * (array1_length + array2_length + 1));
	if (!result)
		return 1;
	i = 0;
	j = 0;
	while (i < array1_length + array2_length) {
		if (i < array1_length)
			result[i] = (*dest)[i];
		else {
			new_str = strdup(src[j]);
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
	free(*dest);
	*dest = result;
	return 0;
}

char	*array_join(array src)
{
	int		i;
	int		total_char;
	char	*result;

	i = 0;
	total_char = 0;
	while (src[i])
		total_char += strlen(src[i++]);
	result = malloc(total_char + 1);
	if (!result)
		return NULL;
	memset(result, 0, total_char + 1);
	i = 0;
	while (src[i])
		strcat(result, src[i++]);
	return result;
}

bool	is_in_array(array src, char *needle)
{
	int	i;

	if (!src)
		return false;
	i = 0;
	while (src[i]) {
		if (strcmp(src[i], needle) == 0)
			return true;
		i++;
	}
	return false;
}

void	array_deduplicate(array *arr)
{
	int		i;
	array	result;

	if (!arr)
		return;
	result = array_init(NULL);
	i = 0;
	while ((*arr)[i]) {
		if (!is_in_array(result, (*arr)[i])) {
			array_append(&result, (*arr)[i]);
		}
		i++;
	}
	free_array(*arr);
	*arr = result;
}
