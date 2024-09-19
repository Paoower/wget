#include "src.h"
#include <stdlib.h>
#include <stdio.h>

char	**get_urls_from_html(char	*file_path)
{
	(void)file_path;
	return NULL;
}

int	wget_mirror(struct parameters_t params)
{
	(void)params;
	return 0;
	// add params.url in urls
	// while urls is not empty
		// for each url
			// append file path from get_file_from_host()

		// empty urls
		// for each file path
			// append urls from get_urls_from_html(file_path) to urls
}
