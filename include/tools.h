#ifndef TOOLS_H
# define TOOLS_H

# include <openssl/ssl.h>
# include <stdbool.h>

typedef char ** arraystr;

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

void			print_arraystr(arraystr arr, char *name);
void			free_arraystr(arraystr array);
void			clean_arraystr(arraystr *array);
int				arraystr_len(arraystr array);
arraystr		arraystr_init(const char *str, ...);
int				arraystr_append(arraystr *dest, char *src);
int				arraystr_merge(arraystr *dest, arraystr src);
char			*arraystr_join(arraystr src);
void			arraystr_deduplicate(arraystr *arr);
bool			is_in_arraystr(arraystr src, char *needle);
// arraystr.c

void			free_array(void **array);
void			clean_array(void ***array);
int				array_len(void **array);
void			**array_init(void *elem, ...);
int				array_append(void ***dest, void *heap_elem);
int				array_merge(void ***dest, void **src);
// array.c

char			*get_str_between(char *src, char *start, char *end);
char			*str_concat(const char *s, ...);
char			*trim_spaces(char *str);
void			copy_string(char **dest, char *src);
// string

int				arg_ptr_len(void *first, va_list args);
// arg.c

int				parse_url();
// parse_url.c

SSL_CTX			*create_ctx_ssl();
SSL				*create_ssl_connection(SSL_CTX *ctx, int sock_fd);
// ssl.c

void			print_current_date(char *text_before);
struct timespec	time_diff(struct timespec time1, struct timespec time2);
struct timespec	get_elapsed_time(struct timespec start_time);
// time.c

char			**split(const char *str, const char *charset);

float			bytes_to_megabytes(int bytes);

#endif
