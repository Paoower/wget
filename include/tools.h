#ifndef TOOLS_H
# define TOOLS_H


// regex.c

int		does_match_with_pattern(const char *str, char *pattern);


// string.c

char	*get_str_between(char *src, char *start, char *end);
char	*str_concat(const char *s, ...);


// parse_url.c

int		parse_url();

#endif
