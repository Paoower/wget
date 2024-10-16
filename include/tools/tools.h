#ifndef TOOLS_H
# define TOOLS_H

# include "conversion.h"
# include "array.h"
# include <openssl/ssl.h>
# include <stdbool.h>

char			*get_file_path(const char *file_name, const char *dir_path);

int				send_http_request(int sock_fd, SSL *ssl,
												char *buffer, int buffer_size);
int				read_http_data(int sock_fd, SSL *ssl,
												char *buffer, int buffer_size);
int				is_ok_status(int status_code);
int				is_redirect_status(int status_code);
char			*get_http_response_info(const char *http_response,
										const char *key, const char *spliter);
// http_request.c

int				does_match_with_pattern(const char *str, char *pattern);
// regex.c

char			*get_str_between(char *src, char *start, char *end);
char			*str_concat(const char *s, ...);
char			*trim_spaces(char *str);
void			copy_string(char **dest, char *src);
// string

int				arg_ptr_len(void *first, va_list args);
// arg.c

int				parse_url();
// parse_url.c

SSL_CTX			*init_ssl();
SSL				*create_ssl_connection(SSL_CTX *ctx,
											int sock_fd, char *hostname);
// ssl.c

void			print_current_date(char *text_before);
struct timespec	time_diff(struct timespec time1, struct timespec time2);
struct timespec	get_elapsed_time(struct timespec start_time);
double			timespec_to_double(struct timespec time);
// time.c

char			**split(const char *str, const char *charset);

#endif
