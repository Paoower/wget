#ifndef ARRAY_H
# define ARRAY_H

# include <stdbool.h>

typedef char ** arraystr;

void		print_arraystr(arraystr arr, char *name);
void		free_arraystr(arraystr array);
void		clean_arraystr(arraystr *array);
int			arraystr_len(arraystr array);
arraystr	arraystr_init(const char *str, ...);
int			arraystr_append(arraystr *dest, char *src);
int			arraystr_merge(arraystr *dest, arraystr src);
char		*arraystr_join(arraystr src);
void		arraystr_deduplicate(arraystr *arr);
bool		is_in_arraystr(arraystr src, char *needle);
// arraystr.c

void		free_array(void **array);
void		clean_array(void ***array);
int			array_len(void **array);
void		**array_init(void *elem, ...);
int			array_append(void ***dest, void *heap_elem);
int			array_merge(void ***dest, void **src);
// array.c

#endif
