#ifndef TOOLS_H
# define TOOLS_H

# include <openssl/ssl.h>

char			*get_file_path(const char *file_name, const char *dir_path);

int				send_http_request(int sock_fd, SSL *ssl,
												char *buffer, int buffer_size);
int				read_http_data(int sock_fd, SSL *ssl,
												char *buffer, int buffer_size);
int				is_redirect_status(const char *status);
char			*get_http_response_info(const char *http_response,
										const char *key, const char *spliter);
// http_request.c

int				does_match_with_pattern(const char *str, char *pattern);
// regex.c

char			*get_str_between(char *src, char *start, char *end);
char			*str_concat(const char *s, ...);
void			copy_string(char **dest, char *src);
// string.c

int				parse_url();
// parse_url.c

void			print_current_date(char *text_before);
struct timespec	time_diff(struct timespec time1, struct timespec time2);
struct timespec	get_elapsed_time(struct timespec start_time);
// time.c

char			**split(const char *str, const char *charset);
void			free_char_tab(char **strs);

float			bytes_to_megabytes(int bytes);

#endif
