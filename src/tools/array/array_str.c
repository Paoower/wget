#include "tools.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void	free_array_str(array_str array)
{
	free_array((void **)array);
}

void	clean_array_str(array_str *array)
{
	clean_array((void ***)array);
}

int	array_str_len(array_str array)
{
	return array_len((void **)array);
}

/**
 * @brief Init array with x values.
 * @param str The list of strings to add. Must be terminated by a NULL.
 * @return
 * Pointer to an array made of the given strings or NULL if the creation fails.
 * Use `free_array_str` or `clean_array_str` to free memory after use.
 */
array_str	array_str_init(const char *str, ...)
{
	va_list		args;
	int			size;
	array_str	result;
	int			i;

	va_start(args, str);
	size = arg_ptr_len((void *)str, args);
	va_end(args);
	if (size == 0)
		return NULL;
	va_start(args, str);
	result = malloc(sizeof(char *) * (size + 1));
	result[size] = NULL;
	i = 0;
	while (str) {
		if (i == size) {
			free_array_str(result);
			return NULL;
		}
		result[i] = strdup(str);
		if (!result[i]) {
			free_array_str(result);
			return NULL;
		}
		i++;
		str = va_arg(args, const char *);
	}
	va_end(args);
	return result;
}

/**
 * @brief Append `elem` to the given array.
 * @param dest The initial array. Must be dynamically created.
 * Put NULL to create an array.
 * @param str The string to add.
 * @return
 * The new array of string with s added,
 * or the initial array if something went wrong.
 * Use `free_array_str` or `clean_array_str` to free memory after use.
 */
int	array_str_append(array_str *dest, char *str)
{
	if (!dest || !str)
		return 1;
	return array_append((void ***)dest, (void *)strdup(str));
}

array_str	array_str_cpy(array_str src)
{
	array_str	result;
	int			i;
	int			src_length;

	if (!src)
		return NULL;
	src_length = array_str_len(src);
	if (src_length <= 0)
		return NULL;
	result = malloc(src_length + 1);
	if (!result)
		return NULL;
	result[src_length] = NULL;
	i = 0;
	while (src[i]) {
		result[i] = strdup(src[i]);
		if (!result[i]) {
			while (i > 0) {
				i--;
				free(result[i]);
			}
			free(result);
			return NULL;
		}
		i++;
	}
	return result;
}

/**
 * @brief Merge the two arrays over array1.
 * Keep every strings from array1 and copy every strings from array2
 * @param heap_array1 The first array. Must be dynamically created.
 * @param array2 The second array.
 * @return A new array that contains the two given arrays.
 * Use `free_array_str` or `clean_array_str` to free memory after use.
 */
int	array_str_merge(array_str *dest, array_str src)
{
	array_str	arr_cpy;

	arr_cpy = array_str_cpy(src);
	if (!arr_cpy)
		return 1;
	array_merge((void ***)dest, (void **)arr_cpy);
	free(arr_cpy); // free container only
	return 0;
}

/**
 * @brief Concatenate every strings from an array to a final string.
 * @param src The source array.
 * @return Pointer to the joined string.
 * The caller is responsible for freeing this memory.
 */
char	*array_str_join(array_str src)
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

bool	is_in_array_str(array_str src, char *needle)
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

void	array_str_deduplicate(array_str *arr)
{
	int			i;
	array_str	result;

	if (!arr)
		return;
	result = array_str_init(NULL);
	i = 0;
	while ((*arr)[i]) {
		if (!is_in_array_str(result, (*arr)[i])) {
			array_str_append(&result, (*arr)[i]);
		}
		i++;
	}
	free_array_str(*arr);
	*arr = result;
}
