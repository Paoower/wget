#include "tools.h"
#include <stdlib.h>
#include <stdarg.h>

/**
 * @brief Free the given array.
 * Do not works properly if the elements
 * are composed of other allocated elements.
 */
void	free_array(void **array)
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
void	clean_array(void ***array)
{
	free_array(*array);
	*array = NULL;
}

int	array_len(void **array)
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
 * @param elem The list of elements to add. Must be terminated by a NULL.
 * @return
 * Pointer to an array made of the given elements or NULL if the creation fails.
 * Use `free_array` or `clean_array` to free memory after use.
 */
void	**array_init(void *heap_elem, ...)
{
	va_list	args;
	int		size;
	void	**result;
	int		i;

	va_start(args, heap_elem);
	size = arg_ptr_len(heap_elem, args);
	va_end(args);
	if (size == 0)
		return NULL;
	va_start(args, heap_elem);
	result = malloc(sizeof(void *) * (size + 1));
	result[size] = NULL;
	i = 0;
	while (heap_elem) {
		if (i == size) {
			free_array(result);
			return NULL;
		}
		result[i++] = heap_elem;
		heap_elem = va_arg(args, void *);
	}
	va_end(args);
	return result;
}

/**
 * @brief Append `elem` to the given array.
 * @param dest The initial array. Must be dynamically created.
 * If NULL, array is created, starting with `heap_elem`.
 * @param elem The element to add. Must be synamically created.
 * @return
 * The new array with the given element added,
 * or the initial array if something went wrong.
 * Use `free_array` or `clean_array` to free memory after use.
 */
int	array_append(void ***dest, void *heap_elem)
{
	int			i;
	int			array_length;
	void		**result;

	if (!dest || !heap_elem)
		return 1;
	array_length = array_len(*dest);
	result = malloc(sizeof(void *) * (array_length + 2));
	if (!result)
		return 1;
	i = 0;
	while (i < array_length) {
		result[i] = (*dest)[i];
		i++;
	}
	result[i++] = heap_elem;
	result[i] = NULL;
	free(*dest);
	*dest = result;
	return 0;
}

int	array_merge(void ***dest, void **src)
{
	int		i;
	int		j;
	int		array1_length;
	int		array2_length;
	void	**result;

	if (!dest)
		return 1;
	array1_length = array_len(*dest);
	array2_length = array_len(src);
	if (array2_length == 0)
		return 1;
	result = malloc(sizeof(void *) * (array1_length + array2_length + 1));
	if (!result)
		return 1;
	i = 0;
	j = 0;
	while (i < array1_length + array2_length) {
		if (i < array1_length)
			result[i] = (*dest)[i];
		else
			result[i] = src[j++];
		i++;
	}
	result[i] = NULL;
	free(*dest);
	*dest = result;
	return 0;
}
