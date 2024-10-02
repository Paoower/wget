#ifndef TOOLS_H
# define TOOLS_H

# include <openssl/ssl.h>
# include <stdbool.h>

typedef char ** array_str;

char			*get_file_path(const char *file_name, const char *dir_path);

int				send_http_request(int sock_fd, SSL *ssl,
												char *buffer, int buffer_size);
int				read_http_data(int sock_fd, SSL *ssl,
												char *buffer, int buffer_size);
int				is_ok_status(const char *status);
int				is_redirect_status(const char *status);
char			*get_http_response_info(const char *http_response,
										const char *key, const char *spliter);
// http_request.c

int				does_match_with_pattern(const char *str, char *pattern);
// regex.c

void			free_array_str(array_str array);
void			clean_array_str(array_str *array);
int				array_str_len(array_str array);
array_str		array_str_init(const char *str, ...);
int				array_str_append(array_str *dest, char *src);
int				array_str_merge(array_str *dest, array_str src);
char			*array_str_join(array_str src);
void			array_str_deduplicate(array_str *arr);
bool			is_in_array_str(array_str src, char *needle);
// array_str.c

void			free_array(void **array);
void			clean_array(void ***array);
int				array_len(void **array);
void			**array_init(void *elem, ...);
int				array_append(void ***dest, void *heap_elem);
int				array_merge(void ***dest, void **src);
// array.c

char			*get_str_between(char *src, char *start, char *end);
char			*str_concat(const char *s, ...);
void			copy_string(char **dest, char *src);
// string

int				arg_ptr_len(void *first, va_list args);
// arg.c

int				parse_url();
// parse_url.c

void			print_current_date(char *text_before);
struct timespec	time_diff(struct timespec time1, struct timespec time2);
struct timespec	get_elapsed_time(struct timespec start_time);
// time.c

char			**split(const char *str, const char *charset);

float			bytes_to_megabytes(int bytes);

#endif
