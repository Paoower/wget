#include "src.h"
#include "tools.h"
#include <stdbool.h>

char	*navigate_after_domain_name(char *link, struct file_data *file_data)
{
	char	*cursor;
	char	*temp_ptr;
	char	*end_link;

	end_link = link + strlen(link);
	cursor = link;
	temp_ptr = strstr(cursor, file_data->host_data->hostname);
	if (temp_ptr)
		cursor = temp_ptr + strlen(file_data->host_data->hostname);
	if (cursor >= end_link)
		return NULL; // cursor out of range
	return cursor;
}

char	*convert_link_to_online(char *link, struct file_data *file_data)
{
	char	*cursor;
	arraystr	arr;
	char	*result;

	cursor = navigate_after_domain_name(link, file_data);
	if (!cursor)
		return NULL;
	if (file_data->host_data->is_secured)
		arr = arraystr_init("https://", file_data->host_data->hostname, NULL);
	else
		arr = arraystr_init("http://", file_data->host_data->hostname, NULL);
	if (cursor[0] == '.' && cursor[1] == '/')
		cursor++;
	if (cursor[0] != '/')
		arraystr_append(&arr, "/");
	arraystr_append(&arr, cursor);
	result = arraystr_join(arr);
	free_arraystr(arr);
	return result;
}

char	*convert_link_to_offline(char *link, struct file_data *file_data)
{
	char	*cursor;

	cursor = navigate_after_domain_name(link, file_data);
	if (!cursor)
		return NULL;
	if (cursor[0] == '.')
		cursor++;
	if (cursor[0] == '/')
		cursor++;
	if (cursor)
		return strdup(cursor);
	return NULL;
}

/**
 * @return Pointer to the formated link.
 * The caller is responsible for freeing this memory.
 */
void	convert_link(char **link, struct file_data *file_data,
											bool convert_links, bool is_mirror)
{
	char	*new_url;

	if (!convert_links)
		return;
	if (!link || !*link || !file_data || !file_data->host_data ||
											!file_data->host_data->hostname)
		return;
	if (is_mirror)
		new_url = convert_link_to_offline(*link, file_data);
	else
		new_url = convert_link_to_online(*link, file_data);
	if (new_url) {
		free(*link);
		*link = new_url;
	}
}
