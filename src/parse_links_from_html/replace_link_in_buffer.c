#include <string.h>
#include <stdlib.h>

char	*get_new_buffer_with_new_link(char	*buffer, char *start_link,
						int link_size, char *new_link, int new_link_size)
{
	char	*new_buffer;
	char	*pos_after_link;
	int		new_buffer_size;

	new_buffer_size = strlen(buffer) - link_size + new_link_size + 1;
	new_buffer = malloc(new_buffer_size);
	if (!new_buffer)
		return NULL;
	memset(new_buffer, 0, new_buffer_size);
	pos_after_link = start_link + link_size;
	strncat(new_buffer, buffer, start_link - buffer); // cpy part before link
	strncat(new_buffer, new_link, new_link_size); // cpy new link
	strncat(new_buffer, pos_after_link, strlen(pos_after_link));
	new_buffer[new_buffer_size - 1] = '\0';
	return new_buffer;
	// cpy part after link
}

/**
 * @param link_quote_pos The pointer to the open quote of the url
 * @return The difference of initial size of `lines` buffer
 * and the final size of `lines` buffer.
 */
int	replace_link_in_buffer(char **lines, char *link_quote_pos, char *new_link)
{
	char	quote;
	char	*start_link;
	char	*end_link;
	int		link_size;
	int		new_link_size;
	char	*new_lines;

	if (!link_quote_pos)
		return 0;
	quote = *link_quote_pos;
	start_link = link_quote_pos + 1;
	end_link = strchr(start_link, quote);
	if (!end_link)
		return 0;
	new_link_size = strlen(new_link);
	if (new_link_size == 0)
		return 0;
	link_size = end_link - start_link;
	new_lines = get_new_buffer_with_new_link(*lines,
								start_link, link_size, new_link, new_link_size);
	if (!new_lines)
		return 0;
	free(*lines);
	*lines = new_lines;
	return new_link_size - link_size;
}
