#ifndef TOOLS_H
# define TOOLS_H

int				does_match_with_pattern(const char *str, char *pattern);
// regex.c

char			*get_str_between(char *src, char *start, char *end);
char			*str_concat(const char *s, ...);
// string.c

int				parse_url();
// parse_url.c

struct timespec	time_diff(struct timespec time1, struct timespec time2);
struct timespec	get_elapsed_time(struct timespec start_time);
// timespec.c

#endif
