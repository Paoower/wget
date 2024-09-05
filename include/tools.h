#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>

#ifndef TOOLS_H
# define TOOLS_H

# define URL_REGEX "^((http|https)://)?(www\\.)?[a-zA-Z0-9\\-\\.]+\\.[a-zA-Z]{2,}(\\:[0-9]{1,5})?(/.*)?$"

// regex.c
int		does_match_with_pattern(const char *str, char *pattern);

// string.c
char	*get_str_between(char *src, char *start, char *end);
char	*concat(const char *s, ...);

// parse_url.c
int		parse_url();

#endif
