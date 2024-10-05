#include "tools.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

void	free_arraystr(arraystr array)
{
	free_array((void **)array);
}

void	clean_arraystr(arraystr *array)
{
	clean_array((void ***)array);
}

int	arraystr_len(arraystr array)
{
	return array_len((void **)array);
}

void	print_arraystr(arraystr arr, char *name)
{
	int	i;

	if (!arr)
		return;
	i = 0;
	if (name)
		printf("%s = ", name);
	printf("[ ");
	while (arr[i]) {
		printf("\"%s\"", arr[i]);
		if (arr[i] && arr[i + 1])
			printf(", ");
		i++;
	}
	printf(" ]\n");
}

/**
 * @brief Init array with x values.
 * @param str The list of strings to add. Must be terminated by a NULL.
 * @return
 * Pointer to an array made of the given strings or NULL if the creation fails.
 * Use `free_array_str` or `clean_array_str` to free memory after use.
 */
arraystr	arraystr_init(const char *str, ...)
{
	va_list		args;
	int			size;
	arraystr	result;
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
			free_arraystr(result);
			return NULL;
		}
		result[i] = strdup(str);
		if (!result[i]) {
			free_arraystr(result);
			return NULL;
		}
		i++;
		str = va_arg(args, const char *);
	}
	va_end(args);
	return result;
}

/**
 * @brief Append `str` to the given array.
 * @param dest The initial array. Must be dynamically created.
 * If NULL, array is created, starting with `str`.
 * @param str The string to add.
 * @return
 * The new array of string with s added,
 * or the initial array if something went wrong.
 * Use `free_array_str` or `clean_array_str` to free memory after use.
 */
int	arraystr_append(arraystr *dest, char *str)
{
	if (!dest || !str)
		return 1;
	return array_append((void ***)dest, (void *)strdup(str));
}

arraystr	arraystr_cpy(arraystr src)
{
	arraystr	result;
	int			i;
	int			src_length;

	if (!src)
		return NULL;
	src_length = arraystr_len(src);
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
int	arraystr_merge(arraystr *dest, arraystr src)
{
	arraystr	arr_cpy;

	arr_cpy = arraystr_cpy(src);
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
char	*arraystr_join(arraystr src)
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

bool	is_in_arraystr(arraystr src, char *needle)
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

void	arraystr_deduplicate(arraystr *arr)
{
	int			i;
	arraystr	result;

	if (!arr || !*arr)
		return;
	result = arraystr_init(NULL);
	i = 0;
	while ((*arr)[i]) {
		if (!is_in_arraystr(result, (*arr)[i])) {
			arraystr_append(&result, (*arr)[i]);
		}
		i++;
	}
	free_arraystr(*arr);
	*arr = result;
}
