#include "src.h"
#include <stdbool.h>

int	convert_link_to_online(char **link, struct file_data *file_data)
{
	// if has not http or htttps
	// if is_secured add https, else http
	(void)link;
	(void)file_data;
	return 0;
}

int	convert_link_to_offline(char **link, struct file_data *file_data)
{
	(void)link;
	(void)file_data;
	return 0;
}

int	convert_link(char **link, struct file_data *file_data, bool is_mirror)
{
	if (!link || !file_data || !file_data->host_data ||
											!file_data->host_data->hostname ||
											!file_data->host_data->is_secured)
		return 1;
	if (is_mirror)
		convert_link_to_offline(link, file_data);
	else
		convert_link_to_online(link, file_data);
	return 0;
}
