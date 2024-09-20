#include "src.h"

int	wget_mirror(struct parameters_t params)
{
	(void)params;
	return 0;
	// add params.url in urls
	// while urls is not empty
		// for each url
			// append file path from get_file_from_host()

		// empty urls tab
		// for each file path
			// append urls from get_urls_from_html(file_path) to urls
			// edit the file and convert links for offline viewing
			// (--convert-links option)
}
