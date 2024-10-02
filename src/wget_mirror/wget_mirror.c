#include "src.h"
#include "tools.h"
#include "download_file_from_url.h"

void	clean_files_data(struct file_data ***file_data)
{
	int	i;

	if (!file_data || !*file_data)
		return;
	i = 0;
	while ((*file_data)[i])
		free_file_data((*file_data)[i]);
	free(*file_data);
	*file_data = NULL;
}

int	wget_mirror(char *url, struct parameters_t params)
{
	array_str			urls;
	array_str			new_urls;
	array_str			dl_history;
	struct file_data	**files_data;
	struct file_data	*file_data;
	int					i;

	urls = array_str_init(url, NULL);
	dl_history = array_str_init(NULL);
	files_data = NULL;
	while (urls) {
		i = 0;
		while (urls[i]) {
			if (!is_in_array_str(dl_history, urls[i])) {
			// if url is not in the dl_history, dl + add
				file_data = download_file_from_url(urls[i], params.storage_path,
						params.output_file, params.rate_limit, params.mirror, 0);
				if (!file_data) {
					i++;
					continue;
				}
				array_append((void ***)&files_data, (void *)file_data);
				array_str_append(&dl_history, urls[i]);
			}
			i++;
		}
		clean_array_str(&urls);
		i = 0;
		while (files_data && files_data[i]) {
			new_urls = get_urls_from_html(files_data[i],
									params.reject_list, params.exclude_list);
			array_str_merge(&urls, new_urls);
			clean_array_str(&new_urls);
			// concatenate every urls in every files
		}
		clean_files_data(&files_data);
		array_str_deduplicate(&urls);
		// remove every duplicated in urls
	}
	clean_array_str(&dl_history);
	return 0;
}
