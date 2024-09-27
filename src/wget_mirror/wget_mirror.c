#include "src.h"
#include "tools.h"
#include "wget_mirror.h"

int	wget_mirror(char *url, struct parameters_t params)
{
	char	**urls;
	char	**new_urls;
	char	**file_paths;
	char	*file_path;
	int		i;

	file_paths = NULL;
	urls = array_append(NULL, url);
	// add url in urls
	while (urls) {
	// while urls is not empty
		i = 0;
		while (urls[i]) {
		// for each url
			file_path = download_file_from_url(urls[i], params.storage_path,
					params.output_file, params.rate_limit, params.mirror, 0);
			file_paths = array_append(file_paths, file_path);
			free(file_path);
			// append file path from download_file_from_url()
			i++;
		}
		free_array(urls);
		urls = NULL;
		// empty urls tab
		i = 0;
		while (file_paths && file_paths[i]) {
		// for each file path
			new_urls = get_urls_from_html(file_paths[i],
									params.reject_list, params.exclude_list);
			urls = array_concat(urls, new_urls);
			// append urls from get_urls_from_html(file_path) to urls
			free_array(new_urls);
		}
	}
	return 0;
}
