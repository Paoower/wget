#ifndef TOOLS_H
# define TOOLS_H

// regex.c
int				does_match_with_pattern(const char *str, char *pattern);

// string.c
char			*get_str_between(char *src, char *start, char *end);
char			*str_concat(const char *s, ...);
void			copy_string(char **dest, char *src);

// parse_url.c
int				parse_url();

// timespec.c
struct timespec	time_diff(struct timespec time1, struct timespec time2);
struct timespec	get_elapsed_time(struct timespec start_time);

#endif
