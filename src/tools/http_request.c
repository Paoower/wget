#include "tools.h"
#include <string.h>
#include <sys/socket.h>


int	send_http_request(int sock_fd, SSL *ssl, char *buffer, int buffer_size)
{
	if (ssl)
		return SSL_write(ssl, buffer, buffer_size);
	else
		return send(sock_fd, buffer, buffer_size, 0);
}

int	read_http_data(int sock_fd, SSL *ssl, char *buffer, int buffer_size)
{
	if (ssl)
		return SSL_read(ssl, buffer, buffer_size);
	else
		return recv(sock_fd, buffer, buffer_size, 0);
}
int	is_ok_status(const char *status)
{
	if (!status)
		return 0;
	return strcmp(status, "200 OK") == 0;
}

int	is_redirect_status(const char *status)
{
	if (!status)
		return 0;
	return (strcmp(status, "301 Moved Permanently") == 0 ||
								strcmp(status, "302 Moved Temporarily") == 0);
}

/**
 * @brief
 * get any content of a line in the http response
 * @param spliter Optionnal string to split the line with (default ": ")
 * @return
 * return the string of the content which match with the key
 * The caller is responsible to free the result.
 */
char	*get_http_response_info(const char *http_response,
										const char *key, const char *spliter)
{
	char	**lines;
	char	**tokens;
	char	*result;
	int		i;

	if (!spliter)
		spliter = ": ";
	lines = split(http_response, "\r\n");
	i = 0;
	while (lines[i]) {
		tokens = split(lines[i], spliter);
		if (tokens) {
			if (tokens[0] && strcasecmp(tokens[0], key) == 0) {
				result = strdup(lines[i] + strlen(tokens[0]) + strlen(spliter));
				free_array(tokens);
				free_array(lines);
				return result;
			}
			free_array(tokens);
		}
		i++;
	}
	free_array(lines);
	return NULL;
}
