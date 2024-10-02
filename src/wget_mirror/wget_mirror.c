#include "src.h"
#include "tools.h"
#include "wget_mirror.h"

int	wget_mirror(char *url, struct parameters_t params)
{
	array				urls;
	array				new_urls;
	array				dl_history;
	array				file_paths;
	struct file_data	*file_data;
	int					i;

	urls = array_init(url, NULL);
	dl_history = array_init(NULL);
	file_paths = array_init(NULL);
	while (urls) {
		i = 0;
		while (urls[i]) {
			if (!is_in_array(dl_history, urls[i])) {
			// if url is not in the dl_history, dl + add
				file_data = download_file_from_url(urls[i], params.storage_path,
						params.output_file, params.rate_limit, params.mirror, 0);
				if (!file_data || !file_data->file_path) {
					free_file_data(file_data);
					continue;
				}
				array_append(&file_paths, file_data->file_path);
				free_file_data(file_data);
				array_append(&dl_history, urls[i]);
			}
			i++;
		}
		clean_array(&urls);
		i = 0;
		while (file_paths && file_paths[i]) {
			new_urls = get_urls_from_html(file_paths[i],
									params.reject_list, params.exclude_list);
			array_merge(&urls, new_urls);
			clean_array(&new_urls);
			// concatenate every urls in every files
		}
		array_deduplicate(&urls);
		// remove every duplicated in urls
	}
	clean_array(&dl_history);
	return 0;
}
